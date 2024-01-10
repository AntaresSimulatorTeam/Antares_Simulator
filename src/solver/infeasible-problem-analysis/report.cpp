/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "report.h"
#include "constraint.h"
#include <antares/logs/logs.h>
#include <algorithm>

using namespace operations_research;

static bool compareSlackSolutions(const Antares::Optimization::Constraint& a,
                                  const Antares::Optimization::Constraint& b)
{
    return a.getSlackValue() > b.getSlackValue();
}

namespace Antares::Optimization
{
InfeasibleProblemReport::InfeasibleProblemReport(const std::vector<const MPVariable*>& slackVariables)
{
    turnSlackVarsIntoConstraints(slackVariables);
    sortConstraints();
    trimConstraints();
}

void InfeasibleProblemReport::turnSlackVarsIntoConstraints(const std::vector<const MPVariable*>& slackVariables)
{
    for (const MPVariable* slack : slackVariables)
    {
        mConstraints.emplace_back(slack->name(), slack->solution_value());
    }
}

void InfeasibleProblemReport::sortConstraints()
{
    std::sort(std::begin(mConstraints), std::end(mConstraints), ::compareSlackSolutions);
}

void InfeasibleProblemReport::trimConstraints()
{
    if (nbVariables <= mConstraints.size())
    {
        mConstraints.resize(nbVariables);
    }
}

void InfeasibleProblemReport::extractItems()
{
    for (auto& c : mConstraints)
    {
        if (c.extractItems() == 0)
        {
            return;
        }
        mTypes[c.getType()]++;
    }
}

void InfeasibleProblemReport::logSuspiciousConstraints()
{
    Antares::logs.error() << "The following constraints are suspicious (first = most suspicious)";
    for (const auto& c : mConstraints)
    {
        Antares::logs.error() << c.prettyPrint();
    }
    Antares::logs.error() << "Possible causes of infeasibility:";
    if (mTypes[ConstraintType::hydro_reservoir_level] > 0)
    {
        Antares::logs.error() << "* Hydro reservoir impossible to manage with cumulative options "
                                 "\"hard bounds without heuristic\"";
    }
    if (mTypes[ConstraintType::fictitious_load] > 0)
    {
        Antares::logs.error() << "* Last resort shedding status,";
    }
    const unsigned int bcCount = mTypes[ConstraintType::binding_constraint_hourly]
                                 + mTypes[ConstraintType::binding_constraint_daily]
                                 + mTypes[ConstraintType::binding_constraint_weekly];
    if (bcCount > 0)
    {
        Antares::logs.error() << "* Binding constraints,";
    }

    Antares::logs.error() << "* Negative hurdle costs on lines with infinite capacity (rare).";
}

void InfeasibleProblemReport::prettyPrint()
{
    extractItems();
    logSuspiciousConstraints();
}

} // namespace Antares::Optimization