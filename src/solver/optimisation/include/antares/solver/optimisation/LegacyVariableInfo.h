// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>

namespace Antares::Optimization
{

// Structured legacy description of an optimization variable, built by VariableNamer
// at the same time as the variable name and stored parallel to NomDesVariables.
struct LegacyVariableInfo
{
    std::string name;
    std::string component;
    unsigned timeIndex;

    // Area the variable belongs to, letting extra outputs key study data by
    // (area, component) when `component` alone is not unique across areas
    // (e.g. two thermal clusters of the same name). Nullopt for link variables,
    // which carry their full "origin$$destination" identity in `component`.
    std::optional<std::string> area;
};

} // namespace Antares::Optimization
