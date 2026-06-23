// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/hydro/management/finalLevelValidator.h"

#include <cmath>

#include "antares/study/study.h"

namespace Antares::Solver
{

void warnIfDefaultFinalLevelsMayBeAdjustedToRespectRuleCurves(
  const Antares::Data::Study& study,
  const std::vector<double>& initialLevels,
  unsigned int year)
{
    study.areas.each(
      [&study, &initialLevels, year](const Data::Area& area)
      {
          if (area.index >= initialLevels.size())
          {
              return;
          }

          const bool finalLevelSetInScenarioBuilder = area.index
                                                        < study.scenarioFinalHydroLevels.width
                                                      && year
                                                           < study.scenarioFinalHydroLevels.height
                                                      && !std::isnan(
                                                        study.scenarioFinalHydroLevels[area.index]
                                                                                      [year]);
          if (finalLevelSetInScenarioBuilder || !area.hydro.reservoirManagement
              || area.hydro.useWaterValue)
          {
              return;
          }

          const double initialLevel = initialLevels[area.index];
          const double lowLevelLastDay = area.hydro.series->ruleCurves.min
                                           .getCoefficient(year, DAYS_PER_YEAR - 1);
          const double highLevelLastDay = area.hydro.series->ruleCurves.max
                                            .getCoefficient(year, DAYS_PER_YEAR - 1);

          if (initialLevel < lowLevelLastDay || initialLevel > highLevelLastDay)
          {
              logs.warning()
                << "Hydro area '" << area.name << "' (year " << year + 1
                << "): Initial reservoir level (" << initialLevel
                << ") is outside the final rule curves [" << lowLevelLastDay << ", "
                << highLevelLastDay
                << "]. The heuristic may produce a final reservoir level different from the "
                   "initial level to respect the rule curves.";
          }
      });
}

FinalLevelValidator::FinalLevelValidator(
  const Antares::Data::PartHydro& hydro,
  const Antares::Data::AreaName areaName, // gp : to std::string
  double initialLevel,
  double finalLevel,
  const unsigned int year,
  const unsigned int lastSimulationDay,
  const unsigned int firstMonthOfSimulation,
  HydroErrorsCollector& errorCollector):
    year_(year),
    lastSimulationDay_(lastSimulationDay),
    firstMonthOfSimulation_(firstMonthOfSimulation),
    hydro_(hydro),
    areaName_(areaName),
    initialLevel_(initialLevel),
    finalLevel_(finalLevel),
    errorCollector_(errorCollector)
{
}

bool FinalLevelValidator::check()
{
    if (skippingFinalLevelUse())
    {
        return true;
    }
    if (!checkForInfeasibility())
    {
        return false;
    }
    finalLevelFineForUse_ = true;
    return true;
}

bool FinalLevelValidator::skippingFinalLevelUse()
{
    return !wasSetInScenarioBuilder() || !compatibleWithReservoirProperties();
}

bool FinalLevelValidator::wasSetInScenarioBuilder()
{
    return !std::isnan(finalLevel_);
}

bool FinalLevelValidator::compatibleWithReservoirProperties()
{
    if (hydro_.reservoirManagement && !hydro_.useWaterValue)
    {
        return true;
    }

    logs.warning()
      << "Final reservoir level not applicable! Year:" << year_ + 1 << ", Area:" << areaName_
      << ". Check: Reservoir management = Yes, Use water values = No and proper initial "
         "reservoir level is provided ";
    return false;
}

bool FinalLevelValidator::checkForInfeasibility()
{
    bool checksOk = hydroAllocationStartMatchesSimulation();
    checksOk = isFinalLevelReachable() && checksOk;
    checksOk = isBetweenRuleCurves() && checksOk;

    return checksOk;
}

bool FinalLevelValidator::hydroAllocationStartMatchesSimulation() const
{
    unsigned initReservoirLvlMonth = hydro_.initializeReservoirLevelDate; // month [0-11]
    if (lastSimulationDay_ == DAYS_PER_YEAR && initReservoirLvlMonth == firstMonthOfSimulation_)
    {
        return true;
    }

    errorCollector_(areaName_) << "Year " << year_ + 1 << ": "
                               << "Hydro allocation must start on the 1st simulation month and "
                               << "simulation last a whole year";

    return false;
}

bool FinalLevelValidator::isFinalLevelReachable() const
{
    double reservoirCapacity = hydro_.reservoirCapacity;
    double totalYearInflows = calculateTotalInflows();

    if ((finalLevel_ - initialLevel_) * reservoirCapacity > totalYearInflows)
    {
        errorCollector_(areaName_)
          << "Year: " << year_ + 1 << " Incompatible total inflows: " << totalYearInflows
          << " with initial: " << initialLevel_ << " and final: " << finalLevel_
          << " reservoir levels.";
        return false;
    }
    return true;
}

double FinalLevelValidator::calculateTotalInflows() const
{
    // calculate yearly inflows
    const auto& srcinflows = hydro_.series->storage.getColumn(year_);

    double totalYearInflows = 0.0;
    for (unsigned int day = 0; day < DAYS_PER_YEAR; ++day)
    {
        totalYearInflows += srcinflows[day];
    }
    return totalYearInflows;
}

bool FinalLevelValidator::isBetweenRuleCurves() const
{
    double lowLevelLastDay = hydro_.series->ruleCurves.min.getCoefficient(year_, DAYS_PER_YEAR - 1);
    double highLevelLastDay = hydro_.series->ruleCurves.max.getCoefficient(year_,
                                                                           DAYS_PER_YEAR - 1);

    if (finalLevel_ < lowLevelLastDay || finalLevel_ > highLevelLastDay)
    {
        errorCollector_(areaName_)
          << "Year: " << year_ + 1 << " Specifed final reservoir level: " << finalLevel_
          << " is incompatible with reservoir level rule curve [" << lowLevelLastDay << " , "
          << highLevelLastDay << "]";
        return false;
    }
    return true;
}

bool FinalLevelValidator::finalLevelFineForUse() const
{
    return finalLevelFineForUse_;
}

} // namespace Antares::Solver
