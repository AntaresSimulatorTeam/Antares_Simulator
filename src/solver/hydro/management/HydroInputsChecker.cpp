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

#include "antares/antares/fatal-error.h"
#include "antares/solver/simulation/common-eco-adq.h"

namespace Antares
{

HydroInputsChecker::HydroInputsChecker(Data::AreaList& areas,
                                       const Data::Parameters& params,
                                       const Date::Calendar& calendar,
                                       Data::SimulationMode simulationMode,
                                       uint firstYear,
                                       uint endYear):
    areas_(areas),
    parameters_(params),
    calendar_(calendar),
    simulationMode_(simulationMode),
    firstYear_(firstYear),
    endYear_(endYear),
    prepareInflows_(areas, calendar),
    minGenerationScaling_(areas, calendar)
{
}

void HydroInputsChecker::Execute()
{
    Antares::Data::Area::ScratchMap scratchmap;
    unsigned int numSpace = 999999;
    uint nbPerformedYears = 0;
    for (auto year = firstYear_; year < endYear_; ++year)
    {
        // performCalculations
        if (parameters_.yearsFilter[year])
        {
            ++nbPerformedYears;
            // Index of the MC year's space (useful if this year is actually run)
            numSpace = nbPerformedYears - 1;
            scratchmap = areas_.buildScratchMap(numSpace);

            PrepareDataFromClustersInMustrunMode(scratchmap, year);

            prepareInflows_.Run(year);
            minGenerationScaling_.Run(year);
            if (!checksOnGenerationPowerBounds(year))
            {
                throw FatalError("hydro management: invalid minimum generation");
            }
            prepareNetDemand(year, parameters_.mode, scratchmap);
        }
    }
}

bool HydroInputsChecker::checksOnGenerationPowerBounds(uint year) const
{
    return (checkMinGeneration(year) && checkGenerationPowerConsistency(year)) ? true : false;
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
    //    const auto& data = tmpDataByArea_.at(&area);
    const auto& data = area.data;
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
    //    const auto& data = tmpDataByArea_.at(&area);
    const auto& data = area.data;
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

void HydroInputsChecker::prepareNetDemand(uint year,
                                          Data::SimulationMode mode,
                                          const Antares::Data::Area::ScratchMap& scratchmap)
{
    areas_.each(
      [this, &year, &scratchmap, &mode](Data::Area& area)
      {
          const auto& scratchpad = scratchmap.at(&area);

          const auto& rormatrix = area.hydro.series->ror;
          const auto* ror = rormatrix.getColumn(year);

          auto& data = area.data;
          const double* loadSeries = area.load.series.getColumn(year);
          const double* windSeries = area.wind.series.getColumn(year);
          const double* solarSeries = area.solar.series.getColumn(year);

          for (uint hour = 0; hour != HOURS_PER_YEAR; ++hour)
          {
              auto dayYear = calendar_.hours[hour].dayYear;

              double netdemand = 0;

              // Aggregated renewable production: wind & solar
              if (parameters_.renewableGeneration.isAggregated())
              {
                  netdemand = +loadSeries[hour] - windSeries[hour] - scratchpad.miscGenSum[hour]
                              - solarSeries[hour] - ror[hour]
                              - ((mode != Data::SimulationMode::Adequacy)
                                   ? scratchpad.mustrunSum[hour]
                                   : scratchpad.originalMustrunSum[hour]);
              }

              // Renewable clusters, if enabled
              else if (parameters_.renewableGeneration.isClusters())
              {
                  netdemand = loadSeries[hour] - scratchpad.miscGenSum[hour] - ror[hour]
                              - ((mode != Data::SimulationMode::Adequacy)
                                   ? scratchpad.mustrunSum[hour]
                                   : scratchpad.originalMustrunSum[hour]);

                  for (auto& c: area.renewable.list.each_enabled())
                  {
                      netdemand -= c->valueAtTimeStep(year, hour);
                  }
              }

              assert(!std::isnan(netdemand)
                     && "hydro management: NaN detected when calculating the net demande");
              data.DLN[dayYear] += netdemand;
          }
      });
}

void HydroInputsChecker::PrepareDataFromClustersInMustrunMode(Data::Area::ScratchMap& scratchmap,
                                                              uint year)
{
    bool inAdequacy = (simulationMode_ == Data::SimulationMode::Adequacy);

    for (uint i = 0; i < areas_.size(); ++i)
    {
        auto& area = *(areas_[i]);
        auto& scratchpad = scratchmap.at(&area);

        memset(scratchpad.mustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);
        if (inAdequacy)
        {
            memset(scratchpad.originalMustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);
        }

        double* mrs = scratchpad.mustrunSum;
        double* adq = scratchpad.originalMustrunSum;

        for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
        {
            const auto& availableProduction = cluster->series.getColumn(year);
            if (inAdequacy && cluster->mustrunOrigin)
            {
                for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
                {
                    mrs[h] += availableProduction[h];
                    adq[h] += availableProduction[h];
                }
            }
            else
            {
                for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
                {
                    mrs[h] += availableProduction[h];
                }
            }
        }

        if (inAdequacy)
        {
            for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
            {
                if (!cluster->mustrunOrigin)
                {
                    continue;
                }

                const auto& availableProduction = cluster->series.getColumn(year);
                for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
                {
                    adq[h] += availableProduction[h];
                }
            }
        }
    }
}

void HydroChecks::Run()
{
    for (auto check: checks)
    {
        check->Run();
    }
}
} // namespace Antares
