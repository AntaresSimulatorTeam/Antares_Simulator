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

    // Per-link transmission capacities in MW, indexed by hour-in-week
    // (0 .. NombreDePasDeTemps - 1). The context is built once per week, so the
    // vectors span the whole week and every block (daily or weekly) reads its
    // own hour. A missing key means the congestion indicators cannot be
    // computed for that link and the rows are skipped.
    std::unordered_map<std::string, std::vector<double>> directCapacityByLink;
    std::unordered_map<std::string, std::vector<double>> indirectCapacityByLink;

    // Per-area residual load in MW, indexed by hour-in-week. Carries the input
    // load series so actual_load can be emitted on the area's UnsuppliedEnergy
    // anchor. A missing key means actual_load is skipped for that area.
    std::unordered_map<std::string, std::vector<double>> loadByArea;

    // Per-area natural hydro inflows in MWh, indexed by hour-in-week. Only the
    // areas with a reservoir carry this series; actual_inflows is emitted on the
    // area's HydroLevel anchor and skipped when the key is missing.
    std::unordered_map<std::string, std::vector<double>> inflowsByArea;

    // Per-link loop flow (origin->extremity) in MW, indexed by hour-in-week.
    // actual_loop_flow is emitted on the link's DirectFlow anchor and skipped
    // when the key is missing.
    std::unordered_map<std::string, std::vector<double>> loopFlowByLink;

    // Absolute time index of the week's first hour (PROBLEME_HEBDO::HeureDansLAnnee,
    // i.e. weekInTheYear * hoursPerWeek). LegacyVariableInfo::timeIndex is
    // absolute, so hourInWeek = timeIndex - weekFirstTimeStep indexes the
    // per-link vectors above.
    unsigned weekFirstTimeStep = 0;
};

} // namespace Antares::Optimization
