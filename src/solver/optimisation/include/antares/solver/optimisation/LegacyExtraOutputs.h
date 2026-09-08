// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <vector>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

class InactiveComponentsAnalyzer;

// Read-only view of the solver solution a legacy fill should publish: the primal
// values (X) and the constraint duals. Passed explicitly so a post-process dump
// can supply the post-processed values without mutating and restoring the solver
// state on PROBLEME_ANTARES_A_RESOUDRE.
struct LegacySolution
{
    const std::vector<double>& primal;
    const std::vector<double>& duals;
};

// Block-relative time index of an absolute hour, empty when the hour falls
// outside the block's global time window. Shared by the raw legacy rows and
// the derived extra outputs so both use the same row conventions.
std::optional<unsigned> LegacyBlockTimeIndex(const LinearProblem::Api::FillContext& fillContext,
                                             unsigned timeIndex);

// Adds the derived "extra outputs" of the legacy solver to the simulation
// table. Iterates the study structure (areas, links, thermal clusters) hour
// by hour and reads the solution (passed in through `solution`) through the
// variable / constraint correspondence tables, so every operand is fetched by
// index; study data (capacities, inflows, emission factors, ...) is read
// straight from problemeHebdo. problemeHebdo is non-const only because the
// correspondence accessors (VariableManager) expose indices as mutable
// references; nothing is written.
void AddLegacyExtraOutputs(IO::Outputs::SimulationTable& simulationTable,
                           PROBLEME_HEBDO& problemeHebdo,
                           const LegacySolution& solution,
                           const LinearProblem::Api::FillContext& fillContext,
                           unsigned currentBlock,
                           const InactiveComponentsAnalyzer* inactiveComponents = nullptr);

} // namespace Antares::Optimization
