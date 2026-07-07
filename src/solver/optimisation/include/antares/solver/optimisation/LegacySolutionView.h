// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "antares/solver/optimisation/LegacyVariableInfo.h"

namespace Antares::Optimization
{

// Lookup of the legacy weekly solution by structured variable identity
// (output name, component, time index), built from the LegacyVariableInfo
// recorded by VariableNamer. Meant to be rebuilt for each simulation-table
// fill: every weekly problem has the same variables at the same indices, but
// the recorded time index is the absolute hour of the year, so the keys
// change from week to week.
//
// Components are looked up by the recorded string as-is; for thermal clusters
// it is the cluster name without the area, so identically-named clusters of
// different areas collide and only one of them is reachable by name.
class LegacySolutionView
{
public:
    LegacySolutionView(const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
                       const std::vector<double>& solutionValues,
                       const std::vector<double>& linearCosts);

    // Solution value of the variable, or nullopt if no such variable was recorded
    std::optional<double> value(const std::string& name,
                                const std::string& component,
                                unsigned timeIndex) const;

    // Linear objective coefficient of the variable, or nullopt if no such variable was recorded
    std::optional<double> linearCost(const std::string& name,
                                     const std::string& component,
                                     unsigned timeIndex) const;

private:
    std::optional<std::size_t> indexOf(const std::string& name,
                                       const std::string& component,
                                       unsigned timeIndex) const;

    const std::vector<double>& solutionValues_;
    const std::vector<double>& linearCosts_;
    std::unordered_map<std::string, std::size_t> indexByKey_;
};

} // namespace Antares::Optimization
