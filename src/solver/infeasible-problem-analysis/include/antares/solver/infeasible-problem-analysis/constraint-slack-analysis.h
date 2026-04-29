// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

#include "unfeasibility-analysis.h"
#include "watched-constraints.h"

namespace operations_research
{
class MPConstraint;
class MPVariable;
class MPSolver;
} // namespace operations_research

namespace Antares::Optimization
{

/*!
 * That particular analysis relaxes all constraints by
 * adding slack variables for each one.
 */
class ConstraintSlackAnalysis: public UnfeasibilityAnalysis
{
public:
    ~ConstraintSlackAnalysis() override = default;

    void run(operations_research::MPSolver* problem) override;
    void printReport() const override;

    std::string title() const override
    {
        return "Slack variables analysis";
    }

    const std::vector<const operations_research::MPVariable*>& largestSlackVariables();

private:
    void selectConstraintsToWatch(operations_research::MPSolver* problem);
    void addSlackVariablesToConstraints(operations_research::MPSolver* problem);
    void buildObjective(operations_research::MPSolver* problem) const;
    void sortSlackVariablesByValue();
    void trimSlackVariables();
    bool anySlackVariableNonZero();

    std::vector<operations_research::MPConstraint*> constraintsToWatch_;
    std::vector<const operations_research::MPVariable*> slackVariables_;
    const double thresholdNonZero = 1e-06;
};

} // namespace Antares::Optimization
