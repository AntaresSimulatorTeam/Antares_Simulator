// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <optional>
#include <string>

#include "antares/solver/lps/LpsFromAntares.h"

namespace Antares::API
{
/**
 * @struct Error
 * @brief The Error structure is used to represent an error that occurred during the simulation.
 */
struct Error
{
    /**
     * @brief The reason for the error.
     */
    std::string reason;
};

/**
 * @struct SimulationResults
 * @brief The SimulationResults structure is used to represent the results of a simulation.
 * @details It contains the path to the simulation, weekly problems, and an optional error.
 */
struct [[nodiscard("Contains results and potential error")]] SimulationResults
{
    /**
     * @brief weekly problems
     */
    Antares::Solver::LpsFromAntares antares_problems;
    /**
     * @brief An optional error that occurred during the simulation.
     */
    std::optional<Error> error;
};

} // namespace Antares::API
