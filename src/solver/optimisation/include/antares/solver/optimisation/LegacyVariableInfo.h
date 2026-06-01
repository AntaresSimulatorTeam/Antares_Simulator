// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace Antares::Optimization
{

struct LegacyVariableInfo
{
    std::string output;
    std::string component;
    unsigned timeIndex;
};

// Extracts the component name from a legacy location encoded as Model<Component_name>.
// Locations may be nested (e.g. "Area<area>::ThermalCluster<continuous_generator_candidate>"),
// in which case the name of the last (innermost) component is kept
// (-> "continuous_generator_candidate"). Returns an empty string when no non-empty
// chevron-delimited content is found.
std::string ExtractLegacyComponentName(const std::string& location);

} // namespace Antares::Optimization
