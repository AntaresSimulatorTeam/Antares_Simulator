// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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
};

} // namespace Antares::Optimization
