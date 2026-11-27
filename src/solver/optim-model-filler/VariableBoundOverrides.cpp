// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optim-model-filler/VariableBoundOverrides.h"

#include <algorithm>

namespace Antares::LinearProblem
{

void VariableBoundOverrides::reset(unsigned firstGlobalTimeStep)
{
    overrides_.clear();
    firstGlobalTimeStep_ = firstGlobalTimeStep;
}

void VariableBoundOverrides::setLowerBounds(const std::string& componentId,
                                            const std::string& variableId,
                                            std::vector<double> values)
{
    overrides_[{componentId, variableId}].lower = std::move(values);
}

void VariableBoundOverrides::setUpperBounds(const std::string& componentId,
                                            const std::string& variableId,
                                            std::vector<double> values)
{
    overrides_[{componentId, variableId}].upper = std::move(values);
}

bool VariableBoundOverrides::contains(const std::string& componentId,
                                      const std::string& variableId) const
{
    return overrides_.contains({componentId, variableId});
}

void VariableBoundOverrides::apply(const std::string& componentId,
                                   const std::string& variableId,
                                   const Api::FillContext& ctx,
                                   std::vector<double>& lb,
                                   std::vector<double>& ub) const
{
    const auto it = overrides_.find({componentId, variableId});
    if (it == overrides_.end())
    {
        return;
    }
    const auto& [lower, upper] = it->second;

    const auto nbTimeSteps = std::min({lb.size(), ub.size(), ctx.getLocalNumberOfTimeSteps()});
    for (std::size_t t = 0; t < nbTimeSteps; ++t)
    {
        const auto globalTimeStep = ctx.getGlobalFirstTimeStep() + t;
        if (globalTimeStep < firstGlobalTimeStep_)
        {
            continue;
        }
        const auto index = globalTimeStep - firstGlobalTimeStep_;
        if (index < lower.size())
        {
            lb[t] = std::max(lb[t], lower[index]);
            ub[t] = std::max(ub[t], lb[t]);
        }
        if (index < upper.size())
        {
            ub[t] = std::min(ub[t], upper[index]);
            lb[t] = std::min(lb[t], ub[t]);
        }
    }
}

} // namespace Antares::LinearProblem
