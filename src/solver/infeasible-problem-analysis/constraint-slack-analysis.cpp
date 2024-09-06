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
#include "antares/solver/infeasible-problem-analysis/constraint-slack-analysis.h"

#include <regex>

#include <antares/logs/logs.h>
#include "antares/solver/infeasible-problem-analysis/report.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

using namespace operations_research;

namespace
{
bool greaterSlackSolutions(const MPVariable* a, const MPVariable* b)
{
    return a->solution_value() > b->solution_value();
}

constexpr unsigned int nbMaxSlackVarsToKeep = 10;
} // namespace

namespace Antares::Optimization
{

void ConstraintSlackAnalysis::run(MPSolver* problem)
{
    selectConstraintsToWatch(problem);
    addSlackVariablesToConstraints(problem);
    if (slackVariables_.empty())
    {
        logs.warning() << title() << " : no constraints have been selected";
        return;
    }

    buildObjective(problem);

    const MPSolver::ResultStatus status = problem->Solve();
    if ((status != MPSolver::OPTIMAL) && (status != MPSolver::FEASIBLE))
    {
        logs.error() << title() << " : modified linear problem could not be solved";
        return;
    }

    sortSlackVariablesByValue();
    trimSlackVariables();
    if (hasDetectedInfeasibilityCause_ = anySlackVariableNonZero(); !hasDetectedInfeasibilityCause_)
    {
        logs.warning() << title() << " : no violation detected for selected constraints.";
    }
}

void ConstraintSlackAnalysis::selectConstraintsToWatch(MPSolver* problem)
{
    ConstraintsFactory factory;
    std::regex rgx = factory.constraintsFilter();
    std::ranges::copy_if(problem->constraints(),
                         std::back_inserter(constraintsToWatch_),
                         [&rgx](auto* c) { return std::regex_search(c->name(), rgx); });
}

void ConstraintSlackAnalysis::addSlackVariablesToConstraints(MPSolver* problem)
{
    /* Optimization:
        We assess that less than 1 every 3 constraint will match
        the regex. If more, push_back may force the copy of memory blocks.
        This should not happen in most cases.
    */
    const unsigned int selectedConstraintsInverseRatio = 3;
    slackVariables_.reserve(problem->NumConstraints() / selectedConstraintsInverseRatio);
    const double infinity = MPSolver::infinity();
    for (MPConstraint* c: constraintsToWatch_)
    {
        if (c->lb() > -infinity)
        {
            const MPVariable* slack = problem->MakeNumVar(0, infinity, c->name() + "::low");
            c->SetCoefficient(slack, 1.);
            slackVariables_.push_back(slack);
        }

        if (c->ub() < infinity)
        {
            const MPVariable* slack = problem->MakeNumVar(0, infinity, c->name() + "::up");
            c->SetCoefficient(slack, -1.);
            slackVariables_.push_back(slack);
        }
    }
}

void ConstraintSlackAnalysis::buildObjective(MPSolver* problem) const
{
    MPObjective* objective = problem->MutableObjective();
    // Reset objective function
    objective->Clear();
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack: slackVariables_)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

void ConstraintSlackAnalysis::sortSlackVariablesByValue()
{
    std::sort(std::begin(slackVariables_), std::end(slackVariables_), ::greaterSlackSolutions);
}

void ConstraintSlackAnalysis::trimSlackVariables()
{
    unsigned int nbSlackVars = slackVariables_.size();
    slackVariables_.resize(std::min(nbMaxSlackVarsToKeep, nbSlackVars));
}

bool ConstraintSlackAnalysis::anySlackVariableNonZero()
{
    return std::ranges::any_of(slackVariables_,
                               [&](auto& v) { return v->solution_value() > thresholdNonZero; });
}

const std::vector<const operations_research::MPVariable*>&
ConstraintSlackAnalysis::largestSlackVariables()
{
    return slackVariables_;
}

void ConstraintSlackAnalysis::printReport() const
{
    InfeasibleProblemReport report(slackVariables_);
    report.storeSuspiciousConstraints();
    report.storeInfeasibilityCauses();
    std::ranges::for_each(report.getLogs(), [](auto& line) { logs.notice() << line; });
}

} // namespace Antares::Optimization
