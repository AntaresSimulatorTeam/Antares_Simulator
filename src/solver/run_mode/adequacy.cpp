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

#include "antares/solver/run_mode/adequacy.h"
#include "antares/solver/simulation/solver.h"
#include "antares/solver/simulation/adequacy.h"

namespace Antares::Solver
{
void runSimulationInAdequacyMode(Antares::Data::Study& study,
                                 const Settings& settings,
                                 Benchmarking::IDurationCollector& durationCollector,
                                 IResultWriter& resultWriter,
                                 Benchmarking::OptimizationInfo& info)
{
    // Type of the simulation
    typedef Solver::Simulation::ISimulation<Solver::Simulation::Adequacy> SimulationType;
    SimulationType simulation(study, settings, durationCollector, resultWriter);
    simulation.checkWriter();
    simulation.run();

    if (!(settings.noOutput || settings.tsGeneratorsOnly))
    {
        Benchmarking::Timer timer;
        simulation.writeResults(/*synthesis:*/ true);
        timer.stop();
        durationCollector.addDuration("synthesis_export", timer.get_duration());

        info = simulation.getOptimizationInfo();
    }
}
} // namespace Antares::Solver