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

FinalLevelInflowsModifier::FinalLevelInflowsModifier() : areaPtr(nullptr)
{
}

void FinalLevelInflowsModifier::fillEmpty()
{
    includeFinalReservoirLevel.push_back(false);
    endLevel.push_back(0.);
    deltaLevel.push_back(0.);
}

void FinalLevelInflowsModifier::initializeGeneralData(const Data::Parameters& parameters, uint year)
{
    simEndDay = parameters.simulationDays.end;
    yearIndex = year;
}

void FinalLevelInflowsModifier::initializePerAreaData(
  const Matrix<double>& scenarioInitialHydroLevels,
  const Matrix<double>& scenarioFinalHydroLevels)
{
    initialReservoirLevel = scenarioInitialHydroLevels[areaPtr->index][yearIndex];
    finalReservoirLevel = scenarioFinalHydroLevels[areaPtr->index][yearIndex];
    deltaReservoirLevel = initialReservoirLevel - finalReservoirLevel;
}

void FinalLevelInflowsModifier::initializePreCheckData()
{
    initReservoirLvlMonth = areaPtr->hydro.initializeReservoirLevelDate; // month [0-11]
    reservoirCapacity = areaPtr->hydro.reservoirCapacity;
}

void FinalLevelInflowsModifier::ruleCurveForSimEndReal()
{
    lowLevelLastDay = areaPtr->hydro.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    highLevelLastDay = areaPtr->hydro.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];
}

void FinalLevelInflowsModifier::assignEndLevelAndDelta()
{
    includeFinalReservoirLevel.at(yearIndex) = true;
    endLevel.at(yearIndex) = finalReservoirLevel;
    deltaLevel.at(yearIndex) = deltaReservoirLevel;
}

double FinalLevelInflowsModifier::calculateTotalInflows() const
{
    // calculate yearly inflows
    const Data::DataSeriesHydro& data = *areaPtr->hydro.series;
    uint tsHydroIndex = data.timeseriesNumbers[0][yearIndex];
    auto& inflowsmatrix = areaPtr->hydro.series->storage;
    auto& srcinflows = inflowsmatrix[tsHydroIndex < inflowsmatrix.width ? tsHydroIndex : 0];

    double totalYearInflows = 0.0;
    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        totalYearInflows += srcinflows[day];
    return totalYearInflows;
}

bool FinalLevelInflowsModifier::preCheckStartAndEndSim() const
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

bool FinalLevelInflowsModifier::preCheckYearlyInflow(double totalYearInflows) const
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

bool FinalLevelInflowsModifier::preCheckRuleCurves() const
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

void FinalLevelInflowsModifier::initializeData(const Matrix<double>& scenarioInitialHydroLevels,
                                               const Matrix<double>& scenarioFinalHydroLevels,
                                               const Data::Parameters& parameters,
                                               uint year)
{
    if (!areaPtr)
        return;
    fillEmpty();
    initializeGeneralData(parameters, year);
    initializePerAreaData(scenarioInitialHydroLevels, scenarioFinalHydroLevels);
    initializePreCheckData();
}

bool FinalLevelInflowsModifier::isActive()
{
    if (areaPtr && areaPtr->hydro.reservoirManagement && !areaPtr->hydro.useWaterValue
        && !isnan(finalReservoirLevel) && !isnan(initialReservoirLevel))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void FinalLevelInflowsModifier::updateInflows()
{
    assignEndLevelAndDelta();
    // rule curve values for simEndDayReal
    ruleCurveForSimEndReal();
}

void FinalLevelInflowsModifier::makeChecks()
{
    bool preChecksPasses = true;

    // pre-check 0 -> simulation must end on day 365 and reservoir level must be
    // initiated in January
    if (!preCheckStartAndEndSim())
        preChecksPasses = false;

    // pre-check 1 -> reservoir_levelDay_365 – reservoir_levelDay_1 ≤
    // yearly_inflows
    if (double totalInflows = calculateTotalInflows();
        !preCheckYearlyInflow(totalInflows))
        preChecksPasses = false;

    // pre-check 2 -> final reservoir level set by the user is within the
    // rule curves for the final day
    if (!preCheckRuleCurves())
        preChecksPasses = false;

    if (!preChecksPasses)
    {
        logs.fatal() << "At least one year has failed final reservoir level pre-checks.";
        AntaresSolverEmergencyShutdown();
    }
}

} // namespace Data
} // namespace Antares