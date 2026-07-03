// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <vector>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"
#include "antares/solver/optimisation/LegacyVariableInfo.h"

namespace Antares::Optimization
{

// Adds the derived "extra outputs" of the legacy solver to the simulation
// table, computed from the weekly solution:
void AddLegacyExtraOutputs(Antares::IO::Outputs::SimulationTable& simulationTable,
                           const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
                           const std::vector<double>& solutionValues,
                           const std::vector<double>& linearCosts,
                           const std::vector<std::optional<LegacyVariableInfo>>& constraintsInfo,
                           const std::vector<double>& constraintDuals,
                           const Antares::LinearProblem::Api::FillContext& fillContext,
                           unsigned currentBlock);

} // namespace Antares::Optimization
