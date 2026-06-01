// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_map>

namespace Antares::Optimization
{

class VariableNameMapper
{
public:
    //! Default mapper: empty map, every name is returned unchanged
    VariableNameMapper() = default;
    //! Build a mapper from a source-name -> mapped-name map
    explicit VariableNameMapper(std::unordered_map<std::string, std::string> outputMap);

    [[nodiscard]] std::string mapOutput(const std::string& name) const;

private:
    std::unordered_map<std::string, std::string> outputMap_;
};

} // namespace Antares::Optimization
