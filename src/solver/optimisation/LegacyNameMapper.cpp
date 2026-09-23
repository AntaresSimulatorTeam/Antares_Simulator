// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyNameMapper.h"

namespace Antares::Optimization
{

const std::unordered_map<std::string, std::string> LegacyNameMapper::variableNameMapping = {
  // Link
  {"DirectFlow", "flow"},
  {"PositiveDirectFlow", "direct_flow"},
  {"PositiveIndirectFlow", "indirect_flow"},

  // Thermal
  {"DispatchableProduction", "generation_power"},
  {"NODU", "num_units_on"},
  {"NumberStartingDispatchableUnits", "num_units_starting"},
  {"NumberBreakingDownDispatchableUnits", "num_units_failing"},
  {"NumberStoppingDispatchableUnits", "num_units_stopping"},

  // Short term storage
  {"Injection", "injection_power"},
  {"Withdrawal", "withdrawal_power"},
  {"Level", "level"},
  {"CostVariationInjection", "injection_variation"},
  {"CostVariationWithdrawal", "withdrawal_variation"},

  // Area
  {"UnsuppliedEnergy", "unsupplied_energy"},
  {"Spillage", "spilled_energy"},

  // Hydro
  {"HydProd", "withdrawal_power"},
  {"Pumping", "injection_power"},
  {"HydroLevel", "level"},
  {"Overflow", "overflow"},
  {"FinalStorage", "final_level"},

  // Identity
};

std::string LegacyNameMapper::mapOutput(const std::string& legacyName) const
{
    if (const auto it = variableNameMapping.find(legacyName); it != variableNameMapping.end())
    {
        return it->second;
    }
    return legacyName;
}

} // namespace Antares::Optimization
