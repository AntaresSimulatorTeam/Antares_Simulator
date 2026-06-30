// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/infeasible-problem-analysis/report.h"

#include <algorithm>
#include <regex>
#include <typeindex>
#include <unordered_set>

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

bool greaterValue(const std::shared_ptr<WatchedConstraint> a, std::shared_ptr<WatchedConstraint> b)
{
    return a->slackValue() > b->slackValue();
}

void InfeasibleProblemReport::filterConstraintsToOneByType()
{
    // 1. Keep only the first constraint encountered for each C++ type, preserving the
    // original relative order. A type_index set replaces the former stable_sort + unique:
    // libstdc++'s std::stable_sort instantiates std::get_temporary_buffer, which is deprecated
    // since C++17 and removed from C++20, so relying on it triggers -Wdeprecated-declarations.
    std::unordered_set<std::type_index> seenTypes;
    std::erase_if(constraints_,
                  [&seenTypes](const std::shared_ptr<WatchedConstraint>& constraint)
                  {
                      // Bind to a reference before typeid so its operand is a side-effect-free
                      // glvalue (the dynamic type is still resolved):
                      // -Wpotentially-evaluated-expression.
                      const WatchedConstraint& ref = *constraint;
                      return !seenTypes.insert(std::type_index(typeid(ref))).second;
                  });
    // 2. Sort the remaining constraints by slack value (in descending order)
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
