// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <cstdint>
#include <vector>

namespace Antares::Optimization
{

// Precomputed, once per study (not per week, not per MC year), flags telling
// whether an area's/link's output rows should be suppressed from the
// simulation table because the underlying object is structurally inactive
// (an entirely-zero time series across the whole loaded chronicle). Indices
// match PROBLEME_HEBDO's `pays`/`interco` numbering.
//
// A freshly constructed analyzer reports every component as active (all
// `*IsAllZero` accessors return false), so it is safe to query before any
// `set*` call has been made for a given index.
class InactiveComponentsAnalyzer
{
public:
    InactiveComponentsAnalyzer() = default;

    void setLoadAllZero(uint32_t pays, bool value);
    void setRorAllZero(uint32_t pays, bool value);
    void setSolarAllZero(uint32_t pays, bool value);
    void setWindAllZero(uint32_t pays, bool value);
    void setMiscGenColumnAllZero(uint32_t pays, unsigned column, bool value);

    [[nodiscard]] bool loadIsAllZero(uint32_t pays) const;
    [[nodiscard]] bool rorIsAllZero(uint32_t pays) const;
    [[nodiscard]] bool solarIsAllZero(uint32_t pays) const;
    [[nodiscard]] bool windIsAllZero(uint32_t pays) const;
    [[nodiscard]] bool miscGenColumnIsAllZero(uint32_t pays, unsigned column) const;

private:
    static bool getFlag(const std::vector<bool>& flags, uint32_t pays);
    static void setFlag(std::vector<bool>& flags, uint32_t pays, bool value);

    std::vector<bool> loadAllZero_;
    std::vector<bool> rorAllZero_;
    std::vector<bool> solarAllZero_;
    std::vector<bool> windAllZero_;
    std::vector<std::vector<bool>> miscGenColumnAllZero_; // [pays][column]
};

} // namespace Antares::Optimization
