
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
