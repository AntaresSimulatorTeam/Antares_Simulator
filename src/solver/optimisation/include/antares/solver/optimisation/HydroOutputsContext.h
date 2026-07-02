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

// Per-area study data the legacy extra outputs need but cannot read off the
// problem's variables (reservoir capacity, natural inflows, residual load).
// Keys are the lowercase area name, matching LegacyVariableInfo::component
// for area-anchored outputs.
struct HydroOutputsContext
{
    // Empty context (used by unit tests and the no-table path).
    HydroOutputsContext() = default;

    // Snapshot the week-wide hydro/load data from problemeHebdo's areas. Built
    // once per week: the snapshotted data is week-wide and constant across the
    // week's daily/weekly blocks.
    explicit HydroOutputsContext(const PROBLEME_HEBDO& problemeHebdo);

    // Reservoir capacity in MWh per area. A missing key or a non-positive
    // value means level_percentage cannot be computed for that area and the
    // row is skipped.
    std::unordered_map<std::string, double> reservoirCapacityByArea;

    // Per-area natural hydro inflows in MWh, indexed by hour-in-week. Only the
    // areas with a reservoir carry this series; actual_inflows is emitted on the
    // area's HydroLevel anchor and skipped when the key is missing.
    std::unordered_map<std::string, std::vector<double>> inflowsByArea;

    // Per-area residual load in MW, indexed by hour-in-week. Carries the input
    // load series so actual_load can be emitted on the area's UnsuppliedEnergy
    // anchor. A missing key means actual_load is skipped for that area.
    // Not hydro data, but computed in the same per-area loop as the fields
    // above; kept here pragmatically to avoid a fourth loop.
    std::unordered_map<std::string, std::vector<double>> loadByArea;
};

} // namespace Antares::Optimization
