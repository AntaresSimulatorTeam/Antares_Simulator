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
#ifndef __SOLVER_VARIABLE_STATE_HXX__
#define __SOLVER_VARIABLE_STATE_HXX__

namespace Antares
{
namespace Solver
{
namespace Variable
{
inline void State::startANewYear()
{
    hourInTheSimulation = 0u;

    memset(thermalClusterProductionForYear, 0, sizeof(thermalClusterProductionForYear));
    memset(thermalClusterOperatingCostForYear, 0, sizeof(thermalClusterOperatingCostForYear));
    memset(reserveParticipationCostForYear, 0, sizeof(reserveParticipationCostForYear));
    memset(thermalClusterNonProportionalCostForYear,
           0,
           sizeof(thermalClusterNonProportionalCostForYear));
    memset(thermalClusterPMinOfTheClusterForYear, 0, sizeof(thermalClusterPMinOfTheClusterForYear));
    memset(thermalClusterDispatchedUnitsCountForYear,
           0,
           sizeof(thermalClusterDispatchedUnitsCountForYear));

    memset(thermalClusterReserveParticipationCostForYear,
            0,
            sizeof(thermalClusterNonProportionalCostForYear));
    memset(STStorageClusterReserveParticipationCostForYear,
            0,
            sizeof(STStorageClusterReserveParticipationCostForYear));
    memset(LTStorageClusterReserveParticipationCostForYear,
            0,
            sizeof(LTStorageClusterReserveParticipationCostForYear));

    // Re-initializing annual costs (to be printed in output into separate files)
    annualSystemCost = 0.;
    optimalSolutionCost1 = 0.;
    optimalSolutionCost2 = 0.;
    averageOptimizationTime1 = 0.;
    averageOptimizationTime2 = 0.;
    averageUpdateTime = 0.;
}

inline void State::yearEndResetThermal()
{
    memset(thermalClusterProductionForYear, 0, sizeof(thermalClusterProductionForYear));
    memset(thermalClusterOperatingCostForYear, 0, sizeof(thermalClusterOperatingCostForYear));
    memset(reserveParticipationCostForYear, 0, sizeof(reserveParticipationCostForYear));
    memset(STStorageClusterReserveParticipationCostForYear,
           0,
           sizeof(STStorageClusterReserveParticipationCostForYear));
    memset(LTStorageClusterReserveParticipationCostForYear,
           0,
           sizeof(LTStorageClusterReserveParticipationCostForYear));
    memset(thermalClusterNonProportionalCostForYear,
           0,
           sizeof(thermalClusterNonProportionalCostForYear));
    memset(thermalClusterPMinOfTheClusterForYear, 0, sizeof(thermalClusterPMinOfTheClusterForYear));
    memset(thermalClusterDispatchedUnitsCountForYear,
           0,
           sizeof(thermalClusterDispatchedUnitsCountForYear));
    memset(thermalClusterReserveParticipationCostForYear,0,sizeof(thermalClusterReserveParticipationCostForYear));
}

inline void State::initFromAreaIndex(const unsigned int areaIndex, uint numSpace)
{
    area = study.areas[areaIndex];
    scratchpad = &area->scratchpad[numSpace];
    thermalCluster = nullptr;

    switch (simulationMode)
    {
    case Data::SimulationMode::Adequacy:
    case Data::SimulationMode::Economy:
    case Data::SimulationMode::Expansion:
    {
        hourlyResults = &problemeHebdo->ResultatsHoraires[areaIndex];
        break;
    }

    case Data::SimulationMode::Unknown:
        break;
    }
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STATE_HXX__
