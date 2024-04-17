/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "antares/study/parts/hydro/finallevelinflowsmodifyer.h"
#include "antares/study/parts/hydro/container.h"

namespace Antares::Data
{

bool isValidLevel(double level)
{
    // gp : why not check level < 100 ?
    return level >= 0. && !isnan(level);
}

FinalLevelInflowsModifier::FinalLevelInflowsModifier(const PartHydro& hydro,
                                                     const unsigned int& areaIndex,
                                                     const AreaName& areaName) :
    hydro_(hydro), areaIndex_(areaIndex), areaName_(areaName)
{
}

bool FinalLevelInflowsModifier::CheckInfeasibility(unsigned int year)
{
    ComputeDelta(year); // gp : we don't need this computation now
    logInfoFinLvlNotApplicable(year); // gp : kind of duplication with isActive() content

    // gp : should be moved up (first line ?) and renamed into doWeSkip()
    // gp : isActive means nothing.
    if (!isActive())
        return true;

    if (!makeChecks(year))
        return false;

    storeDeltaLevels(year);
    return true;
}

void FinalLevelInflowsModifier::ComputeDelta(unsigned int year)
{
    initialLevel_ = (*InitialLevels_)[year];
    finalLevel_ = (*FinalLevels_)[year];
    deltaLevel_ = finalLevel_ - initialLevel_;
}

void FinalLevelInflowsModifier::storeDeltaLevels(unsigned int year)
{
    deltaLevel.at(year) = deltaLevel_;
}

double FinalLevelInflowsModifier::calculateTotalInflows(unsigned int year) const
{
    // calculate yearly inflows
    auto const& srcinflows = hydro_.series->storage.getColumn(year);

    double totalYearInflows = 0.0;
    for (unsigned int day = 0; day < DAYS_PER_YEAR; ++day)
        totalYearInflows += srcinflows[day];
    return totalYearInflows;
}

bool FinalLevelInflowsModifier::hydroAllocationStartMatchesSimulation(unsigned int year) const
{
    int initReservoirLvlMonth = hydro_.initializeReservoirLevelDate; // month [0-11]
    if (lastSimulationDay_ == DAYS_PER_YEAR && initReservoirLvlMonth == firstMonthOfSimulation_)
        return true;

    logs.error() << "Year " << year + 1 << ", area '" << areaName_ << "' : "
                 << "Hydro allocation must start on the 1st simulation month and "
                 << "simulation last a whole year";
    return false;
}

bool FinalLevelInflowsModifier::finalLevelValidity(double totalYearInflows, unsigned int year) const
{
    double reservoirCapacity = hydro_.reservoirCapacity;
    if (deltaLevel_ * reservoirCapacity > totalYearInflows)
    {
        logs.error() << "Year: " << year + 1 << ". Area: " << areaName_
                     << ". Incompatible total inflows: " << totalYearInflows
                     << " with initial: " << initialLevel_
                     << " and final: " << finalLevel_ << " reservoir levels.";
        return false;
    }
    return true;
}

bool FinalLevelInflowsModifier::preCheckRuleCurves(unsigned int year) const
{
    double lowLevelLastDay  = hydro_.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    double highLevelLastDay = hydro_.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];

    if (finalLevel_ < lowLevelLastDay || finalLevel_ > highLevelLastDay)
    {
        logs.error() << "Year: " << year + 1 << ". Area: " << areaName_
                     << ". Specifed final reservoir level: " << finalLevel_
                     << " is incompatible with reservoir level rule curve [" << lowLevelLastDay
                     << " , " << highLevelLastDay << "]";
        return false;
    }
    return true;
}

void FinalLevelInflowsModifier::initialize(const Matrix<double>& scenarioInitialHydroLevels,
                                           const Matrix<double>& scenarioFinalHydroLevels,
                                           const unsigned int lastSimulationDay,
                                           const unsigned int firstMonthOfSimulation,
                                           const unsigned int nbYears)
{
    isApplicable_.assign(nbYears, false);
    deltaLevel.assign(nbYears, 0.);
    InitialLevels_ = &(scenarioInitialHydroLevels.entry[areaIndex_]);
    FinalLevels_ = &(scenarioFinalHydroLevels.entry[areaIndex_]);
    lastSimulationDay_ = lastSimulationDay;
    firstMonthOfSimulation_ = firstMonthOfSimulation;
}

bool FinalLevelInflowsModifier::isActive()
{
    return hydro_.reservoirManagement &&
           !hydro_.useWaterValue &&
           isValidLevel(finalLevel_) &&
           isValidLevel(initialLevel_);
}

// if the user specifies the final reservoir level, but does not specify initial reservoir level
// or uses wrong hydro options
// we should inform the user that the final reservoir level won't be reached
void FinalLevelInflowsModifier::logInfoFinLvlNotApplicable(unsigned int year)
{
    // gp : it's a code duplication with isActive().
    if (isValidLevel(finalLevel_)
        && (!hydro_.reservoirManagement || hydro_.useWaterValue
            || !isValidLevel(initialLevel_)))
        logs.info() << "Final reservoir level not applicable! Year:" << year + 1
                    << ", Area:" << areaName_
                    << ". Check: Reservoir management = Yes, Use water values = No and proper initial "
                       "reservoir level is provided ";
}

bool FinalLevelInflowsModifier::makeChecks(unsigned int year)
{
    // Simulation must end on day 365 and reservoir level must be initiated in January
    bool checksOk = hydroAllocationStartMatchesSimulation(year);

    // Reservoir_levelDay_365 – reservoir_levelDay_1 ≤ yearly_inflows
    double totalInflows = calculateTotalInflows(year);
    // gp : why not call the check : finalLevelReachable()
    checksOk = finalLevelValidity(totalInflows, year) && checksOk;

    // Final reservoir level set by the user is within the
    // rule curves for the final day
    // gp : a better name could be isBetweenRuleCurves(...)
    checksOk = preCheckRuleCurves(year) && checksOk;

    isApplicable_.at(year) = checksOk;

    return checksOk;
}

bool FinalLevelInflowsModifier::isApplicable(unsigned int year)
{
    // If isApplicable_.size() == 0, then instance was not properly initialized
    // and is not applicable.
    return !isApplicable_.empty() && isApplicable_.at(year);
}

} // namespace Antares::Data
