// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyVariableNameParser.h"

#include <string_view>

namespace Antares::Optimization
{
namespace
{
constexpr std::string_view kLegacyNameSeparator = "::";
constexpr std::string_view kLegacyHourTag = "hour";
} // namespace

std::optional<LegacyVariableInfo> ParseLegacyVariableName(const std::string& name)
{
    const auto firstSep = name.find(kLegacyNameSeparator);
    const auto lastSep = name.rfind(kLegacyNameSeparator);
    if (firstSep == std::string::npos || lastSep == std::string::npos || firstSep == lastSep)
    {
        return std::nullopt;
    }

    const std::string output = name.substr(0, firstSep);
    const std::string timePart = name.substr(lastSep + kLegacyNameSeparator.size());
    const std::string location = name.substr(firstSep + kLegacyNameSeparator.size(),
                                             lastSep - firstSep - kLegacyNameSeparator.size());
    if (output.empty() || timePart.empty())
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
    if (unit != kLegacyHourTag)
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

    return LegacyVariableInfo{output, location, timeIndex};
}

} // namespace Antares::Optimization
