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

#include "antares/study/parts/hydro/finalLevelValidator.h"

namespace Antares::Data
{

FinalLevelValidator::FinalLevelValidator(PartHydro& hydro,
                                         unsigned int areaIndex,
                                         const AreaName areaName, // gp : to std::string
                                         double initialLevel,
                                         double finalLevel,
                                         const unsigned int year,
                                         const unsigned int lastSimulationDay,
                                         const unsigned int firstMonthOfSimulation)
    : hydro_(hydro),
    areaName_(areaName),
    areaIndex_(areaIndex),
    initialLevel_(initialLevel),
    finalLevel_(finalLevel),
    year_(year),
    lastSimulationDay_(lastSimulationDay),
    firstMonthOfSimulation_(firstMonthOfSimulation)
{
}

bool FinalLevelValidator::check()
{
    if (skippingFinalLevelUse())
        return true;
    if (! checkForInfeasibility())
        return false;
    finalLevelFineForUse_ = true;
    return true;
}

bool FinalLevelValidator::skippingFinalLevelUse()
{
    if(! wasSetInScenarioBuilder())
        return true;
    if (! compatibleWithReservoirProperties())
        return true;
    return false;
}

bool FinalLevelValidator::wasSetInScenarioBuilder()
{
    return ! isnan(finalLevel_);
}

bool FinalLevelValidator::compatibleWithReservoirProperties()
{
    if (hydro_.reservoirManagement && !hydro_.useWaterValue)
        return true;

    logs.warning() << "Final reservoir level not applicable! Year:" << year_ + 1
                << ", Area:" << areaName_
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
    int initReservoirLvlMonth = hydro_.initializeReservoirLevelDate; // month [0-11]
    if (lastSimulationDay_ == DAYS_PER_YEAR && initReservoirLvlMonth == firstMonthOfSimulation_)
        return true;

    logs.error() << "Year " << year_ + 1 << ", area '" << areaName_ << "' : "
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
        logs.error() << "Year: " << year_ + 1 << ". Area: " << areaName_
                     << ". Incompatible total inflows: " << totalYearInflows
                     << " with initial: " << initialLevel_
                     << " and final: " << finalLevel_ << " reservoir levels.";
        return false;
    }
    return true;
}

double FinalLevelValidator::calculateTotalInflows() const
{
    // calculate yearly inflows
    auto const& srcinflows = hydro_.series->storage.getColumn(year_);

    double totalYearInflows = 0.0;
    for (unsigned int day = 0; day < DAYS_PER_YEAR; ++day)
        totalYearInflows += srcinflows[day];
    return totalYearInflows;
}

bool FinalLevelValidator::isBetweenRuleCurves() const
{
    double lowLevelLastDay  = hydro_.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    double highLevelLastDay = hydro_.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];

    if (finalLevel_ < lowLevelLastDay || finalLevel_ > highLevelLastDay)
    {
        logs.error() << "Year: " << year_ + 1 << ". Area: " << areaName_
                     << ". Specifed final reservoir level: " << finalLevel_
                     << " is incompatible with reservoir level rule curve [" << lowLevelLastDay
                     << " , " << highLevelLastDay << "]";
        return false;
    }
    return true;
}

bool FinalLevelValidator::finalLevelFineForUse()
{
    return finalLevelFineForUse_;
}

} // namespace Antares::Data
