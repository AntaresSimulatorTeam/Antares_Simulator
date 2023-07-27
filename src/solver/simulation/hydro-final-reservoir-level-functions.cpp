/*
** Copyright 2007-2023 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#include "hydro-final-reservoir-level-functions.h"
#include <antares/emergency.h>

namespace Antares::Solver
{

void FinalReservoirLevel(const Matrix<double>& scenarioInitialHydroLevels,
                         const Matrix<double>& scenarioFinalHydroLevels,
                         const Data::Parameters& parameters,
                         std::shared_ptr<Data::FinalLevelInflowsModifyer> finalinflows,
                         uint yearIndex)
{
    bool preChecksPasses = true;
    finalinflows->fillEmpty();
    finalinflows->initializeGeneralData(parameters, yearIndex);
    finalinflows->initializePerAreaData(scenarioInitialHydroLevels, scenarioFinalHydroLevels);

    if (finalinflows->areaPtr->hydro.reservoirManagement
        && !finalinflows->areaPtr->hydro.useWaterValue && !isnan(finalinflows->finalReservoirLevel)
        && !isnan(finalinflows->initialReservoirLevel))
    {
        finalinflows->initializePreCheckData();
        finalinflows->assignEndLevelAndDelta();

        // pre-check 0 -> simulation must end on day 365 and reservoir level must be
        // initiated in January
        if (!finalinflows->preCheckStartAndEndSim())
            preChecksPasses = false;

        // rule curve values for simEndDayReal
        finalinflows->ruleCurveForSimEndReal();

        // calculate (partial)yearly inflows
        const Data::DataSeriesHydro& data = *finalinflows->areaPtr->hydro.series;
        uint tsHydroIndex = data.timeseriesNumbers[0][finalinflows->yearIndex];
        auto& inflowsmatrix = finalinflows->areaPtr->hydro.series->storage;
        auto& srcinflows = inflowsmatrix[tsHydroIndex < inflowsmatrix.width ? tsHydroIndex : 0];

        // pre-check 1 -> reservoir_levelDay_365 – reservoir_levelDay_1 ≤
        // yearly_inflows
        if (double totalInflows = finalinflows->calculateTotalInflows(srcinflows);
            !finalinflows->preCheckYearlyInflow(totalInflows))
            preChecksPasses = false;

        // pre-check 2 -> final reservoir level set by the user is within the
        // rule curves for the final day
        if (!finalinflows->preCheckRuleCurves())
            preChecksPasses = false;
    }
    if (!preChecksPasses)
    {
        logs.fatal() << "At least one year has failed final reservoir level pre-checks.";
        AntaresSolverEmergencyShutdown();
    }
}

} // namespace Antares::Solver