// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/management/HydroInputsChecker.h"

#include <yuni/core/logs.h>

#include <antares/utils/utils.h>
#include "antares/solver/hydro/management/finalLevelValidator.h"
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"

namespace Antares
{
HydroInputsChecker::HydroInputsChecker(Antares::Data::Study& study):
    areas_(study.areas),
    parameters_(study.parameters),
    calendar_(study.calendar),
    prepareInflows_(study.areas, study.calendar),
    minGenerationScaling_(study.areas, study.calendar),
    scenarioInitialHydroLevels_(study.scenarioInitialHydroLevels),
    scenarioFinalHydroLevels_(study.scenarioFinalHydroLevels)
{
}

void HydroInputsChecker::Execute(uint year)
{
    prepareInflows_.loadInflows(year);
    minGenerationScaling_.Run(year);
    if (!checkRuleCurves(year))
    {
        logs.error() << "hydro inputs checks: invalid reservoir levels in year " << year;
    }
    if (!checksOnGenerationPowerBounds(year))
    {
        logs.error() << "hydro inputs checks: invalid minimum generation in year " << year;
    }
    if (parameters_.useCustomScenario)
    {
        CheckFinalReservoirLevelsConfiguration(year);
    }
    prepareInflows_.changeInflowsToAccommodateFinalLevels(year);
}

bool HydroInputsChecker::checksOnGenerationPowerBounds(uint year)
{
    return checkMinGeneration(year) && checkGenerationPowerConsistency(year);
}

bool HydroInputsChecker::checkMinGeneration(uint year)
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

bool HydroInputsChecker::checkRuleCurves(uint year)
{
    bool ret = true;
    areas_.each(
      [this, &ret, &year](const Data::Area& area)
      {
          const auto* minRuleCurves = area.hydro.series->ruleCurves.min.getColumn(year);
          const auto* avgRuleCurves = area.hydro.series->ruleCurves.avg.getColumn(year);
          const auto* maxRuleCurves = area.hydro.series->ruleCurves.max.getColumn(year);

          const auto& tsIndex = area.hydro.series->ruleCurves.timeseriesNumbers[year];

          for (unsigned int day = 0; day < DAYS_PER_YEAR; ++day)
          {
              if (minRuleCurves[day] < 0 || avgRuleCurves[day] < 0
                  || minRuleCurves[day] > maxRuleCurves[day] || avgRuleCurves[day] > 1.
                  || maxRuleCurves[day] > 1.)
              {
                  errorCollector_(area.name)
                    << "Reservoir levels in area " << area.id
                    << " for Time-Serie index:" << tsIndex + 1 << " are invalid on day " << day + 1;
                  ret = false;
                  break;
              }
          }
      });
    return ret;
}

bool HydroInputsChecker::checkWeeklyMinGeneration(uint year, const Data::Area& area)
{
    const auto& srcinflows = area.hydro.series->storage.getColumn(year);
    const auto& srcmingen = area.hydro.series->mingen.getColumn(year);
    // Weekly minimum generation <= Weekly inflows for each week
    bool ret = true;
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
            errorCollector_(area.name)
              << " the minimum generation of " << totalWeekMingen << " MW in week " << week + 1
              << " of TS-" << area.hydro.series->mingen.getSeriesIndex(year) + 1
              << " is incompatible with the inflows of " << totalWeekInflows << " MW.";
            ret = false;
        }
    }
    return ret;
}

bool HydroInputsChecker::checkYearlyMinGeneration(uint year, const Data::Area& area)
{
    const auto& data = area.hydro.managementData.at(year);
    bool ret = true;
    if (data.totalYearMingen > data.totalYearInflows)
    {
        // Yearly minimum generation <= Yearly inflows
        errorCollector_(area.name)
          << " the minimum generation of " << data.totalYearMingen << " MW of TS-"
          << area.hydro.series->mingen.getSeriesIndex(year) + 1
          << " is incompatible with the inflows of " << data.totalYearInflows << " MW.";
        ret = false;
    }
    return ret;
}

bool HydroInputsChecker::checkMonthlyMinGeneration(uint year, const Data::Area& area)
{
    const auto& data = area.hydro.managementData.at(year);
    bool ret = true;
    for (uint month = 0; month != 12; ++month)
    {
        uint realmonth = calendar_.months[month].realmonth;
        // Monthly minimum generation <= Monthly inflows for each month
        if (data.totalMonthMingen[realmonth] > data.totalMonthInflows[realmonth])
        {
            errorCollector_(area.name)
              << " the minimum generation of " << data.totalMonthMingen[realmonth]
              << " MW in month " << month + 1 << " of TS-"
              << area.hydro.series->mingen.getSeriesIndex(year) + 1
              << " is incompatible with the inflows of " << data.totalMonthInflows[realmonth]
              << " MW.";
            ret = false;
        }
    }
    return ret;
}

bool HydroInputsChecker::checkGenerationPowerConsistency(uint year)
{
    bool ret = true;

    areas_.each(
      [this, &ret, &year](const Data::Area& area)
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
                  errorCollector_(area.name)
                    << "In area: " << area.name << " [hourly] minimum generation of " << min
                    << " MW in timestep " << h + 1 << " of TS-" << tsIndexMin + 1
                    << " is incompatible with the maximum generation of " << max
                    << " MW in timestep " << h + 1 << " of TS-" << tsIndexMax + 1 << " MW.";
                  ret = false;
                  return;
              }
          }
      });

    return ret;
}

void HydroInputsChecker::CheckFinalReservoirLevelsConfiguration(uint year)
{
    if (!parameters_.yearsFilter.at(year))
    {
        return;
    }

    areas_.each(
      [this, year](Data::Area& area)
      {
          double initialLevel = scenarioInitialHydroLevels_.entry[area.index][year];
          double finalLevel = scenarioFinalHydroLevels_.entry[area.index][year];

          Antares::Solver::FinalLevelValidator validator(area.hydro,
                                                         area.name,
                                                         initialLevel,
                                                         finalLevel,
                                                         year,
                                                         parameters_.simulationDays.end,
                                                         parameters_.firstMonthInYear,
                                                         errorCollector_);
          if (!validator.check())
          {
              errorCollector_(area.name)
                << "hydro final level : infeasibility for area " << area.name
                << " please check the corresponding final level data (scenario-builder)";
          }
          if (validator.finalLevelFineForUse())
          {
              area.hydro.deltaBetweenFinalAndInitialLevels[year] = finalLevel - initialLevel;
          }
      });
} // End function CheckFinalReservoirLevelsConfiguration

void HydroInputsChecker::CheckForErrors() const
{
    errorCollector_.CheckForErrors();
}
} // namespace Antares
