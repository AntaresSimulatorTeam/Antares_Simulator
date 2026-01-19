// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/solver/simulation/simulation-run.h"

#include "antares/solver/simulation/adequacy.h"
#include "antares/solver/simulation/economy.h"

namespace Antares::Solver
{

template<class simulationType>
Benchmarking::OptimizationInfo runSimulation(Antares::Data::Study& study,
                                             const Settings& settings,
                                             Benchmarking::DurationCollector& durationCollector,
                                             IResultWriter& resultWriter,
                                             Simulation::ISimulationObserver& simulationObserver)
{
    simulationType simulation(study, settings, durationCollector, resultWriter, simulationObserver);
    simulation.checkWriter();
    simulation.run();

    if (!(settings.noOutput || settings.tsGeneratorsOnly))
    {
        durationCollector("synthesis_export")
          << [&simulation] { simulation.writeResults(/*synthesis:*/ true); };

        return simulation.getOptimizationInfo();
    }
    return {};
}

Benchmarking::OptimizationInfo simulationRun(Antares::Data::Study& study,
                                             const Settings& settings,
                                             Benchmarking::DurationCollector& durationCollector,
                                             IResultWriter& resultWriter,
                                             Simulation::ISimulationObserver& simulationObserver)
{
    // gp : is there a reason why we do that here and not inside the clusters and
    // gp : right after the read step ?
    // gp : It would allow to move a piece of code out of the Study.
    study.computePThetaInfForThermalClusters();

    switch (study.runtime.mode)
    {
    case Data::SimulationMode::Adequacy:
        return runSimulation<Solver::Simulation::ISimulation<Solver::Simulation::Adequacy>>(
          study,
          settings,
          durationCollector,
          resultWriter,
          simulationObserver);
    case Data::SimulationMode::Economy:
    case Data::SimulationMode::Expansion:
    default:
        return runSimulation<Solver::Simulation::ISimulation<Solver::Simulation::Economy>>(
          study,
          settings,
          durationCollector,
          resultWriter,
          simulationObserver);
    }
}

} // namespace Antares::Solver
