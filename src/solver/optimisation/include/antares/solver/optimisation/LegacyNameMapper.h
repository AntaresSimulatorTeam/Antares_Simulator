// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <unordered_map>

namespace Antares::Optimization
{

class LegacyNameMapper
{
public:
    [[nodiscard]] std::string mapOutput(const std::string& legacyName) const;

private:
    static const std::unordered_map<std::string, std::string> variableNameMapping;
};

} // namespace Antares::Optimization
