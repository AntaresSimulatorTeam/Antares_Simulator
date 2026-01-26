
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
  const std::filesystem::path& output,
  const Antares::Solver::Optimization::OptimizationOptions& optOptions) noexcept;
} // namespace Antares::API
