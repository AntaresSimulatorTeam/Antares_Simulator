/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/solver/hydro/management/HydroInputsChecker.h"

#include <yuni/core/logs.h>

#include <antares/utils/utils.h>
#include "antares/antares/fatal-error.h"
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"

namespace Antares
{

HydroInputsChecker::HydroInputsChecker(Antares::Data::Study& study):
    areas_(study.areas),
    parameters_(study.parameters),
    calendar_(study.calendar),
    simulationMode_(study.runtime->mode),
    firstYear_(0),
    endYear_(1 + study.runtime->rangeLimits.year[Data::rangeEnd]),
    prepareInflows_(study.areas, study.calendar),
    minGenerationScaling_(study.areas, study.calendar)
{
}

void HydroInputsChecker::Execute(uint year)
{
    // for (auto year = firstYear_; year < endYear_; ++year)
    // {
    // unecessary, done by the caller ?
    if (parameters_.yearsFilter[year])
    {
        prepareInflows_.Run(year);
        minGenerationScaling_.Run(year);
        if (!checksOnGenerationPowerBounds(year))
        {
            throw FatalError("hydro inputs checks: invalid minimum generation");
        }
    }
    // }
}

bool HydroInputsChecker::checksOnGenerationPowerBounds(uint year) const
{
    return checkMinGeneration(year) && checkGenerationPowerConsistency(year);
}

bool HydroInputsChecker::checkMinGeneration(uint year) const
{
    bool ret = true;
    areas_.each(
      [this, &ret, &year](const Data::Area& area)
      {
          bool useHeuristicTarget = area.hydro.useHeuristicTarget;
          bool followLoadModulations = area.hydro.followLoadModulations;
          bool reservoirManagement = area.hydro.reservoirManagement;

          if (!useHeuristicTarget)
          {
              return;
          }

          if (!followLoadModulations)
          {
              ret = checkWeeklyMinGeneration(year, area) && ret;
              return;
          }

          if (reservoirManagement)
          {
              ret = checkYearlyMinGeneration(year, area) && ret;
          }
          else
          {
              ret = checkMonthlyMinGeneration(year, area) && ret;
          }
      });
    return ret;
}

bool HydroInputsChecker::checkWeeklyMinGeneration(uint year, const Data::Area& area) const
{
    const auto& srcinflows = area.hydro.series->storage.getColumn(year);
    const auto& srcmingen = area.hydro.series->mingen.getColumn(year);
    // Weekly minimum generation <= Weekly inflows for each week
    for (uint week = 0; week < calendar_.maxWeeksInYear - 1; ++week)
    {
        double totalWeekMingen = 0.0;
        double totalWeekInflows = 0.0;
        for (uint hour = calendar_.weeks[week].hours.first;
             hour < calendar_.weeks[week].hours.end && hour < HOURS_PER_YEAR;
             ++hour)
        {
            totalWeekMingen += srcmingen[hour];
        }

        for (uint day = calendar_.weeks[week].daysYear.first;
             day < calendar_.weeks[week].daysYear.end;
             ++day)
        {
            totalWeekInflows += srcinflows[day];
        }
        if (totalWeekMingen > totalWeekInflows)
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << totalWeekMingen << " MW in week " << week + 1 << " of TS-"
                         << area.hydro.series->mingen.getSeriesIndex(year) + 1
                         << " is incompatible with the inflows of " << totalWeekInflows << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroInputsChecker::checkYearlyMinGeneration(uint year, const Data::Area& area) const
{
    const auto& data = area.hydro.managementData.at(year);
    if (data.totalYearMingen > data.totalYearInflows)
    {
        // Yearly minimum generation <= Yearly inflows
        logs.error() << "In Area " << area.name << " the minimum generation of "
                     << data.totalYearMingen << " MW of TS-"
                     << area.hydro.series->mingen.getSeriesIndex(year) + 1
                     << " is incompatible with the inflows of " << data.totalYearInflows << " MW.";
        return false;
    }
    return true;
}

bool HydroInputsChecker::checkMonthlyMinGeneration(uint year, const Data::Area& area) const
{
    const auto& data = area.hydro.managementData.at(year);
    for (uint month = 0; month != 12; ++month)
    {
        uint realmonth = calendar_.months[month].realmonth;
        // Monthly minimum generation <= Monthly inflows for each month
        if (data.totalMonthMingen[realmonth] > data.totalMonthInflows[realmonth])
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << data.totalMonthMingen[realmonth] << " MW in month " << month + 1
                         << " of TS-" << area.hydro.series->mingen.getSeriesIndex(year) + 1
                         << " is incompatible with the inflows of "
                         << data.totalMonthInflows[realmonth] << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroInputsChecker::checkGenerationPowerConsistency(uint year) const
{
    bool ret = true;

    areas_.each(
      [&ret, &year](const Data::Area& area)
      {
          const auto& srcmingen = area.hydro.series->mingen.getColumn(year);
          const auto& srcmaxgen = area.hydro.series->maxHourlyGenPower.getColumn(year);

          const uint tsIndexMin = area.hydro.series->mingen.getSeriesIndex(year);
          const uint tsIndexMax = area.hydro.series->maxHourlyGenPower.getSeriesIndex(year);

          for (uint h = 0; h < HOURS_PER_YEAR; ++h)
          {
              const auto& min = srcmingen[h];
              const auto& max = srcmaxgen[h];

              if (max < min)
              {
                  logs.error() << "In area: " << area.name << " [hourly] minimum generation of "
                               << min << " MW in timestep " << h + 1 << " of TS-" << tsIndexMin + 1
                               << " is incompatible with the maximum generation of " << max
                               << " MW in timestep " << h + 1 << " of TS-" << tsIndexMax + 1
                               << " MW.";
                  ret = false;
                  return;
              }
          }
      });

    return ret;
}
} // namespace Antares
