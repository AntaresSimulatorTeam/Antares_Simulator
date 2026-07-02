// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <array>
#include <span>
#include <string>
#include <unordered_map>

#include "antares/study/parts/thermal/pollutant.h"

// Forward-declared so the constructor below can take a PROBLEME_HEBDO without
// dragging the full simulation problem into this header: the unit tests build
// a minimal context by setting the public members directly.
struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

// Per-cluster thermal data for the emissions and margin outputs
// (co2_emissions..., cluster_availability, up_margin, min_gen_power,
// down_margin), keyed "area$$cluster": the thermal anchor's component is the
// cluster name alone, which is not unique across areas, so the area qualifier
// on LegacyVariableInfo is folded into the key. unitSize, minStablePower and
// availability/minGenPower are the spinning-adjusted quantities the weekly
// problem already carries; the spec's raw cluster_max_generation /
// max_power_per_unit differ from them by the same (1 - spinning/100) factor,
// which cancels in cluster_availability = max(availability, minStablePower *
// ceil(availability / unitSize)). A missing key (or a per-hour vector shorter
// than the week) means the corresponding outputs are skipped for that
// cluster.
struct ThermalClusterData
{
    ThermalClusterData() = default;
    ThermalClusterData(std::array<double, Data::Pollutant::POLLUTANT_MAX> emissionFactors,
                       double unitSize,
                       double minStablePower,
                       std::span<const double> availability,
                       std::span<const double> minGenPower);
    // Per-pollutant emission factors (tons/MWh).
    std::array<double, Data::Pollutant::POLLUTANT_MAX> emissionFactors{};
    // Nominal capacity per unit, with spinning (TailleUnitaireDUnGroupe...).
    double unitSize = 0.;
    // Min stable power per unit (PminDuPalierThermiquePendantUneHeure).
    double minStablePower = 0.;
    // Available power per hour-in-week (PuissanceDisponibleDuPalierThermique).
    // Non-owning: spans PROBLEME_HEBDO's vector, so the context must not
    // outlive the PROBLEME_HEBDO it was built from.
    std::span<const double> availability;
    // Min generation per hour-in-week (PuissanceMinDuPalierThermique), i.e.
    // min(availability, modulation-based floor); equals the spec's min(., M)
    // once clamped against availability. Non-owning, same lifetime constraint
    // as availability above.
    std::span<const double> minGenPower;
};

struct ThermalOutputsContext
{
    // Empty context (used by unit tests and the no-table path).
    ThermalOutputsContext() = default;

    // Snapshot the week-wide thermal per-cluster data from problemeHebdo's
    // areas. Built once per week: the snapshotted data is week-wide and
    // constant across the week's daily/weekly blocks.
    explicit ThermalOutputsContext(const PROBLEME_HEBDO& problemeHebdo);

    std::unordered_map<std::string, ThermalClusterData> byCluster;
};

} // namespace Antares::Optimization
