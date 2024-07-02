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

#include <antares/logs/logs.h>
#include "antares/solver/infeasible-problem-analysis/constraint.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

using namespace operations_research;

static bool compareSlackSolutions(const Antares::Optimization::Constraint& a,
                                  const Antares::Optimization::Constraint& b)
{
    return a.getSlackValue() > b.getSlackValue();
}

namespace Antares::Optimization
{
InfeasibleProblemReport::InfeasibleProblemReport(
    const std::vector<const MPVariable*>& slackVariables)
{
    turnSlackVarsIntoConstraints(slackVariables);
    sortConstraints();
    trimConstraints();
}

void InfeasibleProblemReport::turnSlackVarsIntoConstraints(
    const std::vector<const MPVariable*>& slackVariables)
{
    for (const MPVariable* slack: slackVariables)
    {
        constraints_.emplace_back(slack->name(), slack->solution_value());
    }
}

void InfeasibleProblemReport::sortConstraints()
{
    std::sort(std::begin(constraints_), std::end(constraints_), ::compareSlackSolutions);
}

void InfeasibleProblemReport::trimConstraints()
{
    unsigned int nbConstraints = constraints_.size();
    constraints_.resize(std::min(nbMaxVariables, nbConstraints));
}

void InfeasibleProblemReport::sortConstraintsByType()
{
    for (auto& c: constraints_)
    {
        if (c.extractComponentsFromName() == 0)
        {
            return;
        }
        nbConstraintsByType_[c.getType()]++;
    }
}

void InfeasibleProblemReport::logSuspiciousConstraints()
{
    Antares::logs.error() << "The following constraints are suspicious (first = most suspicious)";
    for (const auto& c: constraints_)
    {
        Antares::logs.error() << c.prettyPrint();
    }
    Antares::logs.error() << "Possible causes of infeasibility:";
    if (nbConstraintsByType_[ConstraintType::hydro_reservoir_level] > 0)
    {
        Antares::logs.error() << "* Hydro reservoir impossible to manage with cumulative options "
                                 "\"hard bounds without heuristic\"";
    }
    if (nbConstraintsByType_[ConstraintType::fictitious_load] > 0)
    {
        Antares::logs.error() << "* Last resort shedding status,";
    }
    if (nbConstraintsByType_[ConstraintType::short_term_storage_level] > 0)
    {
        Antares::logs.error()
          << "* Short-term storage reservoir level impossible to manage. Please check inflows, "
             "lower & upper curves and initial level (if prescribed),";
    }

    const unsigned int bcCount = nbConstraintsByType_[ConstraintType::binding_constraint_hourly]
                                 + nbConstraintsByType_[ConstraintType::binding_constraint_daily]
                                 + nbConstraintsByType_[ConstraintType::binding_constraint_weekly];
    if (bcCount > 0)
    {
        Antares::logs.error() << "* Binding constraints,";
    }

    Antares::logs.error() << "* Negative hurdle costs on lines with infinite capacity (rare).";
}

void InfeasibleProblemReport::prettyPrint()
{
    sortConstraintsByType();
    logSuspiciousConstraints();
}

} // namespace Antares::Optimization
