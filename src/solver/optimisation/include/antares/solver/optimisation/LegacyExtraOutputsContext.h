// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Antares::Optimization
{

// Study-data lookups the legacy extra outputs need when the formula uses a
// parameter that is NOT an objective coefficient on a recorded variable (and
// therefore cannot be read through LegacySolutionView::linearCost).
//
// The struct is populated by the caller from PROBLEME_HEBDO once per
// FillLegacySimulationTable call and consumed by AddLegacyExtraOutputs. It
// decouples the extra-output code from PROBLEME_HEBDO so the unit tests can
// build a minimal context without dragging the full problem in.
//
// Lookup keys mirror LegacyVariableInfo::component verbatim:
//   - area-anchored outputs use the lowercase area name;
//   - link-anchored outputs use "origin$$destination" (see AREA_SEP in
//     opt_rename_problem.cpp).
struct LegacyExtraOutputsContext
{
    // Reservoir capacity in MWh per area. A missing key or a non-positive
    // value means level_percentage cannot be computed for that area and the
    // row is skipped.
    std::unordered_map<std::string, double> reservoirCapacityByArea;

    // Per-link transmission capacities in MW, indexed by within-block pdt
    // (0 .. NombreDePasDeTempsPourUneOptimisation - 1). A missing key means
    // the congestion indicators cannot be computed for that link and the rows
    // are skipped.
    std::unordered_map<std::string, std::vector<double>> directCapacityByLink;
    std::unordered_map<std::string, std::vector<double>> indirectCapacityByLink;
};

} // namespace Antares::Optimization
