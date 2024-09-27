/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
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
