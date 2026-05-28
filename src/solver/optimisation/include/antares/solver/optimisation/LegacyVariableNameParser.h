// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>

namespace Antares::Optimization
{

struct LegacyVariableInfo
{
    std::string output;
    std::string component;
    unsigned timeIndex;
};

std::optional<LegacyVariableInfo> ParseLegacyVariableName(const std::string& name);

} // namespace Antares::Optimization
