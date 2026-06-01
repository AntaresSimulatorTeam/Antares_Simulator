// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyVariableNameParser.h"

#include <string_view>

namespace Antares::Optimization
{
namespace
{
constexpr std::string_view LEGACY_NAME_SEPARATOR = "::";
constexpr std::string_view LEGACY_HOUR_TAG = "hour";

// Extracts the component name from a legacy location encoded as Model<Component_name>.
// Locations may be nested (e.g. "Area<area>::ThermalCluster<continuous_generator_candidate>"),
// in which case the name of the last (innermost) component is kept
// (-> "continuous_generator_candidate"). Returns an empty string when no non-empty
// chevron-delimited content is found.
std::string ExtractComponentName(const std::string& location)
{
    const auto lt = location.rfind('<');
    if (lt == std::string::npos)
    {
        return {};
    }
    const auto gt = location.find('>', lt);
    if (gt == std::string::npos || gt <= lt + 1)
    {
        return {};
    }
    return location.substr(lt + 1, gt - lt - 1);
}
} // namespace

std::optional<LegacyVariableInfo> ParseLegacyVariableName(const std::string& name)
{
    const auto firstSep = name.find(LEGACY_NAME_SEPARATOR);
    const auto lastSep = name.rfind(LEGACY_NAME_SEPARATOR);
    if (firstSep == std::string::npos || lastSep == std::string::npos || firstSep == lastSep)
    {
        return std::nullopt;
    }

    const std::string output = name.substr(0, firstSep);
    const std::string timePart = name.substr(lastSep + LEGACY_NAME_SEPARATOR.size());
    const std::string location = name.substr(firstSep + LEGACY_NAME_SEPARATOR.size(),
                                             lastSep - firstSep - LEGACY_NAME_SEPARATOR.size());
    if (output.empty() || timePart.empty())
    {
        return std::nullopt;
    }

    // The legacy location is encoded as Model<Component_name> (e.g. Thermal<nuc-fr>).
    // Only the component name inside the chevrons is kept (e.g. nuc-fr).
    const std::string component = ExtractComponentName(location);
    if (component.empty())
    {
        return std::nullopt;
    }

    const auto lt = timePart.find('<');
    const auto gt = timePart.rfind('>');
    if (lt == std::string::npos || gt == std::string::npos || gt <= lt + 1)
    {
        return std::nullopt;
    }

    const std::string unit = timePart.substr(0, lt);
    if (unit != LEGACY_HOUR_TAG)
    {
        return std::nullopt;
    }

    const std::string value = timePart.substr(lt + 1, gt - lt - 1);
    unsigned timeIndex = 0;
    try
    {
        timeIndex = static_cast<unsigned>(std::stoul(value));
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }

    return LegacyVariableInfo{output, component, timeIndex};
}

} // namespace Antares::Optimization
