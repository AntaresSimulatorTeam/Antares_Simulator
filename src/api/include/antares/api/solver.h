
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

#include <antares/optimization-options/options.h>

#include "SimulationResults.h"

namespace Antares::API
{
/**
 * @brief The PerformSimulation function is used to perform a simulation.
 * @param study_path The path to the study to be simulated.
 * @return SimulationResults object which contains the results of the simulation.
 * @exception noexcept This function does not throw exceptions.
 */
SimulationResults PerformSimulation(
  const std::filesystem::path& study_path,
  const Antares::Solver::Optimization::OptimizationOptions& optOptions) noexcept;
} // namespace Antares::API
