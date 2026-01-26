// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STATE_HXX__
#define __SOLVER_VARIABLE_STATE_HXX__

namespace Antares::Solver::Variable
{
inline void State::startANewYear()
{
    hourInTheSimulation = 0u;

    memset(thermalClusterProductionForYear, 0, sizeof(thermalClusterProductionForYear));
    memset(thermalClusterOperatingCostForYear, 0, sizeof(thermalClusterOperatingCostForYear));
    memset(thermalClusterNonProportionalCostForYear,
           0,
           sizeof(thermalClusterNonProportionalCostForYear));
    memset(thermalClusterPMinOfTheClusterForYear, 0, sizeof(thermalClusterPMinOfTheClusterForYear));
    memset(thermalClusterDispatchedUnitsCountForYear,
           0,
           sizeof(thermalClusterDispatchedUnitsCountForYear));

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
    memset(thermalClusterNonProportionalCostForYear,
           0,
           sizeof(thermalClusterNonProportionalCostForYear));
    memset(thermalClusterPMinOfTheClusterForYear, 0, sizeof(thermalClusterPMinOfTheClusterForYear));
    memset(thermalClusterDispatchedUnitsCountForYear,
           0,
           sizeof(thermalClusterDispatchedUnitsCountForYear));
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

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_STATE_HXX__
