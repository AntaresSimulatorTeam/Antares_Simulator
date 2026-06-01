// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/VariableNameMapper.h"

#include <utility>

namespace Antares::Optimization
{

VariableNameMapper::VariableNameMapper(std::unordered_map<std::string, std::string> outputMap):
    outputMap_(std::move(outputMap))
{
}

std::string VariableNameMapper::mapOutput(const std::string& name) const
{
    if (const auto it = outputMap_.find(name); it != outputMap_.end())
    {
        return it->second;
    }
    return name;
}

} // namespace Antares::Optimization
