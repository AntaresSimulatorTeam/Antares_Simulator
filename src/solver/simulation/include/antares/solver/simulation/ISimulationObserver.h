
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

#include "sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{

/**
 * @class ISimulationObserver
 * @brief The ISimulationObserver class is an interface for observing the simulation.
 * @details It declares the notifyHebdoProblem method.
 */
class ISimulationObserver
{
public:
    virtual ~ISimulationObserver() = default;
    /**
     * @brief The notifyHebdoProblem method is used to notify of a problem during the simulation.
     * @param problemeHebdo A pointer to a PROBLEME_HEBDO object representing the problem.
     * @param optimizationNumber The number of the optimization.
     * @param name The name of the problem.
     */
    virtual void notifyHebdoProblem(const PROBLEME_HEBDO& problemeHebdo,
                                    int optimizationNumber,
                                    std::string_view name)
      = 0;
};

/**
 * @class NullSimulationObserver
 * @brief The NullSimulationObserver class is a null object for the ISimulationObserver interface.
 * @details It overrides the notifyHebdoProblem method with an empty implementation.
 */
class NullSimulationObserver: public ISimulationObserver
{
public:
    ~NullSimulationObserver() override = default;

    void notifyHebdoProblem(const PROBLEME_HEBDO&, int, std::string_view) override
    {
        // null object pattern
    }
};
} // namespace Antares::Solver::Simulation
