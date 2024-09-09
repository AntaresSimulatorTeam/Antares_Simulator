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

#pragma once
#include <antares/solver/lps/LpsFromAntares.h>
#include <antares/solver/simulation/ISimulationObserver.h>

namespace Antares::API
{

/**
 * @class SimulationObserver
 * @brief The SimulationObserver class is used to observe the simulation.
 * @details It inherits from the ISimulationObserver interface and overrides the notifyHebdoProblem
 * method.
 */
class SimulationObserver: public Solver::Simulation::ISimulationObserver
{
public:
    /**
     * @brief Used to notify of a solver HEBDO_PROBLEM.
     * HEBDO_PROBLEM is assumed to be properly constructed and valid in order to build
     * LpsFromAntares properly
     * @param problemeHebdo A pointer to a PROBLEME_HEBDO object representing the problem.
     * @param optimizationNumber The number of the optimization.
     * @param name The name of the problem.
     */
    void notifyHebdoProblem(const PROBLEME_HEBDO& problemeHebdo,
                            int optimizationNumber,
                            std::string_view name) override;

    /**
     * @brief The acquireLps method is used to take ownership of Antares problems.
     * @return An LpsFromAntares object containing the linear programming problems.
     */
    Solver::LpsFromAntares&& acquireLps() noexcept;

private:
    Solver::LpsFromAntares lps_;
    mutable std::mutex lps_mutex_;
    mutable std::once_flag flag_;
};

} // namespace Antares::API
