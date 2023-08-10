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

#include "finallevelinflowsmodifyer.h"
#include "container.h"

namespace Antares
{
namespace Data
{
FinalLevelInflowsModifier::FinalLevelInflowsModifier(const PartHydro& hydro,
                                                     const unsigned int& areaIndex,
                                                     const AreaName& areaName) :
    hydro(hydro), 
    areaIndex(areaIndex), 
    areaName(areaName)
{
}

void FinalLevelInflowsModifier::setLastSiumlationDay(uint day)
{
    lastSimulationDay_ = day;
}

void FinalLevelInflowsModifier::setCurrentYear(uint year)
{
    yearIndex = year;
}

void FinalLevelInflowsModifier::ComputeDeltaForCurrentYear()
{
    initialReservoirLevel = (*scenarioInitialHydroLevels_)[areaIndex][yearIndex];
    finalReservoirLevel = (*scenarioFinalHydroLevels_)[areaIndex][yearIndex];
    deltaReservoirLevel = initialReservoirLevel - finalReservoirLevel;
}

void FinalLevelInflowsModifier::updateInflows()
{
    includeFinalReservoirLevel.at(yearIndex) = true;
    deltaLevel.at(yearIndex) = deltaReservoirLevel;
}

double FinalLevelInflowsModifier::calculateTotalInflows() const
{
    // calculate yearly inflows
    const Data::DataSeriesHydro& data = *hydro.series;
    uint tsHydroIndex = data.timeseriesNumbers[0][yearIndex];
    auto& inflowsmatrix = hydro.series->storage;
    auto& srcinflows = inflowsmatrix[tsHydroIndex < inflowsmatrix.width ? tsHydroIndex : 0];

    double totalYearInflows = 0.0;
    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        totalYearInflows += srcinflows[day];
    return totalYearInflows;
}

bool FinalLevelInflowsModifier::preCheckStartAndEndSim() const
{
    
    int initReservoirLvlMonth = hydro.initializeReservoirLevelDate; // month [0-11]
    if (lastSimulationDay_ == DAYS_PER_YEAR && initReservoirLvlMonth == 0)
        return true;
    else
    {
        logs.error() << "Year: " << yearIndex + 1 << ". Area: " << areaName
                     << ". Simulation must end on day 365 and reservoir level must be "
                        "initiated in January";
        return false;
    }
}

bool FinalLevelInflowsModifier::preCheckYearlyInflow(double totalYearInflows) const
{
    double reservoirCapacity = hydro.reservoirCapacity;
    if ((-deltaReservoirLevel) * reservoirCapacity
        > totalYearInflows) // ROR time-series in MW (power), SP time-series in MWh
                            // (energy)
    {
        logs.error() << "Year: " << yearIndex + 1 << ". Area: " << areaName
                     << ". Incompatible total inflows: " << totalYearInflows
                     << " with initial: " << initialReservoirLevel
                     << " and final: " << finalReservoirLevel << " reservoir levels.";
        return false;
    }
    return true;
}

bool FinalLevelInflowsModifier::preCheckRuleCurves() const
{
    double lowLevelLastDay  = hydro.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    double highLevelLastDay = hydro.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];

    if (finalReservoirLevel < lowLevelLastDay || finalReservoirLevel > highLevelLastDay)
    {
        logs.error() << "Year: " << yearIndex + 1 << ". Area: " << areaName
                     << ". Specifed final reservoir level: " << finalReservoirLevel
                     << " is incompatible with reservoir level rule curve [" << lowLevelLastDay
                     << " , " << highLevelLastDay << "]";
        return false;
    }
    return true;
}

void FinalLevelInflowsModifier::initialize(const Matrix<double>& scenarioInitialHydroLevels,
                                           const Matrix<double>& scenarioFinalHydroLevels,
                                           const uint lastSimulationDay,
                                           const uint nbYears)
{
    includeFinalReservoirLevel = std::move(std::vector<bool>(nbYears, false));
    deltaLevel = std::move(std::vector<double>(nbYears, 0.));

    scenarioInitialHydroLevels_ = &scenarioInitialHydroLevels;
    scenarioFinalHydroLevels_ = &scenarioFinalHydroLevels;

    setLastSiumlationDay(lastSimulationDay);
}

void FinalLevelInflowsModifier::initialize(uint year)
{
    setCurrentYear(year);
    ComputeDeltaForCurrentYear();
}


bool FinalLevelInflowsModifier::isActive()
{
    return  hydro.reservoirManagement && 
            !hydro.useWaterValue &&
            finalReservoirLevel >= 0. &&
            initialReservoirLevel >= 0.;
}

bool FinalLevelInflowsModifier::makeChecks()
{
    // Simulation must end on day 365 and reservoir level must be
    // initiated in January
    bool checksOk = preCheckStartAndEndSim();

    // Reservoir_levelDay_365 – reservoir_levelDay_1 ≤ yearly_inflows
    double totalInflows = calculateTotalInflows();
    checksOk = preCheckYearlyInflow(totalInflows) && checksOk;

    // Final reservoir level set by the user is within the
    // rule curves for the final day
    checksOk = preCheckRuleCurves() && checksOk;

    return checksOk;
}

} // namespace Data
} // namespace Antares