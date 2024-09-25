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
#include "antares/solver/infeasible-problem-analysis/report.h"

#include <algorithm>
#include <regex>
#include <typeindex>

namespace Antares::Optimization
{
InfeasibleProblemReport::InfeasibleProblemReport(
  const std::vector<const operations_research::MPVariable*>& slackVariables)
{
    buildConstraintsFromSlackVars(slackVariables);
}

void InfeasibleProblemReport::buildConstraintsFromSlackVars(
  const std::vector<const operations_research::MPVariable*>& slackVariables)
{
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
    const WatchedConstraint* a_raw = a.get();
    const WatchedConstraint* b_raw = b.get();
    // TODO Compiler-dependent behavior
    return std::type_index(typeid(*a_raw)) < std::type_index(typeid(*b_raw));
}

bool sameType(const std::shared_ptr<WatchedConstraint> a,
              const std::shared_ptr<WatchedConstraint> b)
{
    const WatchedConstraint* a_raw = a.get();
    const WatchedConstraint* b_raw = b.get();
    return std::type_index(typeid(*a_raw)) == std::type_index(typeid(*b_raw));
}

bool greaterValue(const std::shared_ptr<WatchedConstraint> a, std::shared_ptr<WatchedConstraint> b)
{
    return a->slackValue() > b->slackValue();
}

void InfeasibleProblemReport::filterConstraintsToOneByType()
{
    // 1. Grouping constraints by C++ type (inside a group, order of instances remains unchanged)
    std::ranges::stable_sort(constraints_, lessTypeName);
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

std::vector<std::string> InfeasibleProblemReport::getLogs()
{
    return report_;
}

} // namespace Antares::Optimization
