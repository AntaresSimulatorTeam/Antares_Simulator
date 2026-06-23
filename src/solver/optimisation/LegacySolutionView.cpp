// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacySolutionView.h"

namespace Antares::Optimization
{

namespace
{
// '\x1F' (ASCII unit separator) cannot appear in names or component
// identifiers, so the concatenation is collision-free.
std::string MakeKey(const std::string& name, const std::string& component, unsigned timeIndex)
{
    return name + '\x1F' + component + '\x1F' + std::to_string(timeIndex);
}
} // namespace

LegacySolutionView::LegacySolutionView(
  const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
  const std::vector<double>& solutionValues,
  const std::vector<double>& linearCosts):
    solutionValues_(solutionValues),
    linearCosts_(linearCosts)
{
    for (std::size_t index = 0; index < variablesInfo.size(); ++index)
    {
        if (const auto& info = variablesInfo[index])
        {
            indexByKey_.emplace(MakeKey(info->name, info->component, info->timeIndex), index);
        }
    }
}

std::optional<std::size_t> LegacySolutionView::indexOf(const std::string& name,
                                                       const std::string& component,
                                                       unsigned timeIndex) const
{
    if (const auto it = indexByKey_.find(MakeKey(name, component, timeIndex));
        it != indexByKey_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

std::optional<double> LegacySolutionView::value(const std::string& name,
                                                const std::string& component,
                                                unsigned timeIndex) const
{
    const auto index = indexOf(name, component, timeIndex);
    if (index && *index < solutionValues_.size())
    {
        return solutionValues_[*index];
    }
    return std::nullopt;
}

std::optional<double> LegacySolutionView::linearCost(const std::string& name,
                                                     const std::string& component,
                                                     unsigned timeIndex) const
{
    const auto index = indexOf(name, component, timeIndex);
    if (index && *index < linearCosts_.size())
    {
        return linearCosts_[*index];
    }
    return std::nullopt;
}

} // namespace Antares::Optimization
