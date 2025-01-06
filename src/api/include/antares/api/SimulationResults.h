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
