// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyNameMapper.h"

namespace Antares::Optimization
{

const std::unordered_map<std::string, std::string> LegacyNameMapper::kOutputMap = {
  {"UnsuppliedEnergy", "unsupplied_energy"},
  {"Spillage", "spilled_energy"},
};

std::string LegacyNameMapper::mapOutput(const std::string& legacyName) const
{
    if (const auto it = kOutputMap.find(legacyName); it != kOutputMap.end())
    {
        return it->second;
    }
    return legacyName;
}

} // namespace Antares::Optimization
