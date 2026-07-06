// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include "antares/study/parts/thermal/pollutant.h"

// Forward-declared so the constructor below can take a PROBLEME_HEBDO without
// dragging the full simulation problem into this header: the unit tests build a
// minimal context by setting the public members directly (see the note below).
struct PROBLEME_HEBDO;

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
    // Empty context (used by unit tests and the no-table path).
    LegacyExtraOutputsContext() = default;

    // Snapshot the week-wide study data the legacy extra outputs need but cannot
    // read off the problem's variables (reservoir capacities, link NTC, loads,
    // inflows, thermal margins...). Built once per week from problemeHebdo: the
    // snapshotted data is week-wide and constant across the week's daily/weekly
    // blocks. Keys mirror the `component` field on LegacyVariableInfo (lowercased
    // area names; for links, "origin$$destination" matching AREA_SEP in
    // opt_rename_problem.cpp).
    explicit LegacyExtraOutputsContext(const PROBLEME_HEBDO& problemeHebdo);

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

    // Per-cluster pollutant emission factors (tons/MWh), keyed by
    // "area$$cluster": the thermal anchor's component is the cluster name alone,
    // which is not unique across areas, so the area qualifier on
    // LegacyVariableInfo is folded into the key. A missing key means the
    // emissions outputs are skipped for that cluster.
    std::unordered_map<std::string, std::array<double, Data::Pollutant::POLLUTANT_MAX>>
      emissionFactorsByCluster;

    // Per-cluster thermal data for the margin outputs (cluster_availability,
    // up_margin, min_gen_power, down_margin), keyed "area$$cluster" like the
    // emission factors. These are the spinning-adjusted quantities the weekly
    // problem already carries; the spec's raw cluster_max_generation /
    // max_power_per_unit differ from them by the same (1 - spinning/100) factor,
    // which cancels in cluster_availability = max(availability,
    // minStablePower * ceil(availability / unitSize)). A missing key (or a
    // per-hour vector shorter than the week) means the margin outputs are
    // skipped for that cluster.
    struct ThermalMarginData
    {
        ThermalMarginData(double unitSize,
                          double minStablePower,
                          const std::vector<double>& availability,
                          const std::vector<double>& minGenPower);
        // Nominal capacity per unit, with spinning (TailleUnitaireDUnGroupe...).
        double unitSize = 0.;
        // Min stable power per unit (PminDuPalierThermiquePendantUneHeure).
        double minStablePower = 0.;
        // Available power per hour-in-week (PuissanceDisponibleDuPalierThermique).
        const std::vector<double>& availability;
        // Min generation per hour-in-week (PuissanceMinDuPalierThermique), i.e.
        // min(availability, modulation-based floor); equals the spec's
        // min(., M) once clamped against availability.
        const std::vector<double>& minGenPower;
    };

    std::unordered_map<std::string, ThermalMarginData> thermalMarginByCluster;

    // Absolute time index of the week's first hour (PROBLEME_HEBDO::HeureDansLAnnee,
    // i.e. weekInTheYear * hoursPerWeek). LegacyVariableInfo::timeIndex is
    // absolute, so hourInWeek = timeIndex - weekFirstTimeStep indexes the
    // per-link vectors above.
    unsigned weekFirstTimeStep = 0;
};

} // namespace Antares::Optimization
