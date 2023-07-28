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

namespace Antares
{
namespace Data
{

FinalLevelInflowsModifyer::FinalLevelInflowsModifyer() : areaPtr(nullptr) {}

void FinalLevelInflowsModifyer::fillEmpty()
{
    includeFinalReservoirLevel.push_back(false);
    endLevel.push_back(0.);
    deltaLevel.push_back(0.);
}

void FinalLevelInflowsModifyer::initializeGeneralData(const Data::Parameters& parameters, uint year)
{
    simEndDay = parameters.simulationDays.end;
    yearIndex = year;
}

void FinalLevelInflowsModifyer::initializePerAreaData(
  const Matrix<double>& scenarioInitialHydroLevels,
  const Matrix<double>& scenarioFinalHydroLevels)
{
    initialReservoirLevel = scenarioInitialHydroLevels[areaPtr->index][yearIndex];
    finalReservoirLevel = scenarioFinalHydroLevels[areaPtr->index][yearIndex];
    deltaReservoirLevel = initialReservoirLevel - finalReservoirLevel;
}

void FinalLevelInflowsModifyer::initializePreCheckData()
{
    initReservoirLvlMonth = areaPtr->hydro.initializeReservoirLevelDate; // month [0-11]
    reservoirCapacity = areaPtr->hydro.reservoirCapacity;
}

void FinalLevelInflowsModifyer::ruleCurveForSimEndReal()
{
    lowLevelLastDay = areaPtr->hydro.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    highLevelLastDay = areaPtr->hydro.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];
}

void FinalLevelInflowsModifyer::assignEndLevelAndDelta()
{
    includeFinalReservoirLevel.at(yearIndex) = true;
    endLevel.at(yearIndex) = finalReservoirLevel;
    deltaLevel.at(yearIndex) = deltaReservoirLevel;
}

double FinalLevelInflowsModifyer::calculateTotalInflows(
  Antares::Memory::Stored<double>::Type& srcinflows) const
{
    double totalYearInflows = 0.0;
    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        totalYearInflows += srcinflows[day];
    return totalYearInflows;
}

bool FinalLevelInflowsModifyer::preCheckStartAndEndSim() const
{
    if (simEndDay == DAYS_PER_YEAR && initReservoirLvlMonth == 0)
        return true;
    else
    {
        logs.error() << "Year: " << yearIndex + 1 << ". Area: " << areaPtr->name
                     << ". Simulation must end on day 365 and reservoir level must be "
                        "initiated in January";
        return false;
    }
}

bool FinalLevelInflowsModifyer::preCheckYearlyInflow(double totalYearInflows) const
{
    if ((-deltaReservoirLevel) * reservoirCapacity
        > totalYearInflows) // ROR time-series in MW (power), SP time-series in MWh
                            // (energy)
    {
        logs.error() << "Year: " << yearIndex + 1 << ". Area: " << areaPtr->name
                     << ". Incompatible total inflows: " << totalYearInflows
                     << " with initial: " << initialReservoirLevel
                     << " and final: " << finalReservoirLevel << " reservoir levels.";
        return false;
    }
    return true;
}

bool FinalLevelInflowsModifyer::preCheckRuleCurves() const
{
    if (finalReservoirLevel < lowLevelLastDay || finalReservoirLevel > highLevelLastDay)
    {
        logs.error() << "Year: " << yearIndex + 1 << ". Area: " << areaPtr->name
                     << ". Specifed final reservoir level: " << finalReservoirLevel
                     << " is incompatible with reservoir level rule curve [" << lowLevelLastDay
                     << " , " << highLevelLastDay << "]";
        return false;
    }
    return true;
}

} // namespace Data
} // namespace Antares