// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/infeasible-problem-analysis/report.h"

#include <algorithm>
#include <regex>

namespace Antares::Optimization
{
InfeasibleProblemReport::InfeasibleProblemReport(
  const std::vector<const operations_research::MPVariable*>& slackVariables)
{
    // Build constraints from slack variables (one constraint per slack variable)
    const ConstraintsFactory constraintsFactory;
    for (const auto* slackVar: slackVariables)
    {
        auto constraint = constraintsFactory.create(slackVar->name(), slackVar->solution_value());
        if (constraint)
        {
            constraints_.push_back(std::move(constraint));
        }
    }
}

bool lessTypeName(const std::shared_ptr<WatchedConstraint> a,
                  const std::shared_ptr<WatchedConstraint> b)
{
    // TODO Compiler-dependent behavior
    // Dereference before applying typeid so its operand is a side-effect-free
    // glvalue (the dynamic type is still resolved): -Wpotentially-evaluated-expression.
    const WatchedConstraint& ra = *a;
    const WatchedConstraint& rb = *b;
    return typeid(ra).before(typeid(rb));
}

bool sameType(const std::shared_ptr<WatchedConstraint> a,
              const std::shared_ptr<WatchedConstraint> b)
{
    const WatchedConstraint& ra = *a;
    const WatchedConstraint& rb = *b;
    return typeid(ra) == typeid(rb);
}

bool greaterValue(const std::shared_ptr<WatchedConstraint> a, std::shared_ptr<WatchedConstraint> b)
{
    return a->slackValue() > b->slackValue();
}

void InfeasibleProblemReport::filterConstraintsToOneByType()
{
    // 1. Grouping constraints by C++ type (inside a group, order of instances remains unchanged)
    // NOTE: libstdc++'s std::stable_sort instantiates std::get_temporary_buffer, which is
    // deprecated since C++17. The deprecation comes from the standard library internals, not
    // from our code; silence it locally. std::sort is not an option here because stability is
    // required (step 2 keeps the first instance of each type group).
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    std::ranges::stable_sort(constraints_, lessTypeName);
#pragma GCC diagnostic pop
    // 2. Keeping the first instances of each group, and rejecting others (= duplicates) to the end.
    auto duplicates = std::ranges::unique(constraints_, sameType);
    // 3. Removing trailing duplicates
    constraints_.erase(duplicates.begin(), duplicates.end());
    // 4. Sorting remaining constraints by slack value (in descending order)
    std::ranges::sort(constraints_, greaterValue);
}

void InfeasibleProblemReport::storeSuspiciousConstraints()
{
    report_.push_back("Violated constraints:");
    for (const auto& c: constraints_)
    {
        report_.push_back(c->infeasibility());
    }
}

void InfeasibleProblemReport::storeInfeasibilityCauses()
{
    filterConstraintsToOneByType();
    report_.push_back("Possible causes of infeasibility:");
    for (const auto& c: constraints_)
    {
        report_.push_back(c->infeasibilityCause());
    }
}

const std::vector<std::string>& InfeasibleProblemReport::getLogs() const
{
    return report_;
}

} // namespace Antares::Optimization
