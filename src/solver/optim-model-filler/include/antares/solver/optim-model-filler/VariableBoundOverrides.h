// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>

namespace Antares::LinearProblem
{

/**
 * Bounds computed outside of the model (typically by a thermal heuristic between two
 * optimisations) that tighten the bounds of time-dependent component variables when the
 * optimisation problem is built.
 *
 * Values are indexed by time step, index 0 being the global time step given to reset().
 * A lower bound override never loosens the lower bound of the model, and raises the upper bound if
 * needed to keep the variable feasible (and conversely for an upper bound override).
 */
class VariableBoundOverrides
{
public:
    /// Removes all overrides, and sets the global time step of index 0
    void reset(unsigned firstGlobalTimeStep);

    void setLowerBounds(const std::string& componentId,
                        const std::string& variableId,
                        std::vector<double> values);
    void setUpperBounds(const std::string& componentId,
                        const std::string& variableId,
                        std::vector<double> values);

    [[nodiscard]] bool contains(const std::string& componentId,
                                const std::string& variableId) const;

    [[nodiscard]] bool empty() const
    {
        return overrides_.empty();
    }

    /**
     * Applies the overrides of a variable to its bounds.
     * @param lb, ub bounds of the variable, indexed by local time step of ctx
     */
    void apply(const std::string& componentId,
               const std::string& variableId,
               const Api::FillContext& ctx,
               std::vector<double>& lb,
               std::vector<double>& ub) const;

private:
    struct Bounds
    {
        std::vector<double> lower;
        std::vector<double> upper;
    };

    std::map<std::pair<std::string, std::string>, Bounds> overrides_;
    unsigned firstGlobalTimeStep_ = 0;
};

} // namespace Antares::LinearProblem
