/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
