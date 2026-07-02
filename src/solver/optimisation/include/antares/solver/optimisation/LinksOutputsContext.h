// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// Forward-declared so the constructor below can take a PROBLEME_HEBDO without
// dragging the full simulation problem into this header: the unit tests build
// a minimal context by setting the public members directly.
struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

// Per-link study data the legacy extra outputs need but cannot read off the
// problem's variables (transmission capacities, loop flow). Keys are
// "origin$$destination" (see AREA_SEP in opt_rename_problem.cpp), matching
// LegacyVariableInfo::component for link-anchored outputs.
struct LinksOutputsContext
{
    // Empty context (used by unit tests and the no-table path).
    LinksOutputsContext() = default;

    // Snapshot the week-wide link data from problemeHebdo's interconnections.
    // Built once per week: the snapshotted data is week-wide and constant
    // across the week's daily/weekly blocks.
    explicit LinksOutputsContext(const PROBLEME_HEBDO& problemeHebdo);

    // Per-link transmission capacities in MW, indexed by hour-in-week
    // (0 .. NombreDePasDeTemps - 1). A missing key means the congestion
    // indicators cannot be computed for that link and the rows are skipped.
    std::unordered_map<std::string, std::vector<double>> directCapacityByLink;
    std::unordered_map<std::string, std::vector<double>> indirectCapacityByLink;

    // Per-link loop flow (origin->extremity) in MW, indexed by hour-in-week.
    // actual_loop_flow is emitted on the link's DirectFlow anchor and skipped
    // when the key is missing.
    std::unordered_map<std::string, std::vector<double>> loopFlowByLink;
};

} // namespace Antares::Optimization
