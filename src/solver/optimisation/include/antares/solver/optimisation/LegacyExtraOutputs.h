// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

// Block-relative time index of an absolute hour, empty when the hour falls
// outside the block's global time window. Shared by the raw legacy rows and
// the derived extra outputs so both use the same row conventions.
std::optional<unsigned> LegacyBlockTimeIndex(
  const LinearProblem::Api::FillContext& fillContext,
  unsigned timeIndex);

// Adds the derived "extra outputs" of the legacy solver to the simulation
// table. Iterates the study structure (areas, links, thermal clusters) hour
// by hour and reads the solved problem through the variable / constraint
// correspondence tables, so every operand is fetched by index; study data
// (capacities, inflows, emission factors, ...) is read straight from
// problemeHebdo. problemeHebdo is non-const only because the correspondence
// accessors (VariableManager) expose indices as mutable references; nothing
// is written.
void AddLegacyExtraOutputs(IO::Outputs::SimulationTable& simulationTable,
                           PROBLEME_HEBDO& problemeHebdo,
                           const LinearProblem::Api::FillContext& fillContext,
                           unsigned currentBlock);

} // namespace Antares::Optimization
