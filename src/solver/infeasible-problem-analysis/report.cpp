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

#include <antares/logs/logs.h>

namespace Antares::Optimization
{
InfeasibleProblemReport::InfeasibleProblemReport(
  const std::vector<const MPVariable*>& slackVariables)
{
    buildConstraintsFromSlackVars(slackVariables);
    filterConstraintsToOneByType();
}

void InfeasibleProblemReport::buildConstraintsFromSlackVars(
  const std::vector<const MPVariable*>& slackVariables)
{
    const ConstraintsFactory constraintsFactory;
    for (const auto& slackVar: slackVariables)
    {
        constraints_.push_back(constraintsFactory.create(slackVar->name()));
    }
}

void InfeasibleProblemReport::filterConstraintsToOneByType()
{
    std::vector<std::string> pickedTypes;
    for (const auto& c: constraints_)
    {
        std::string cType = typeid(*c).name();
        if (std::find(pickedTypes.begin(), pickedTypes.end(), cType) == pickedTypes.end())
        {
            uniqueConstraintByType_.push_back(c);
            pickedTypes.push_back(cType);
        }
    }
}

void InfeasibleProblemReport::logSuspiciousConstraints()
{
    for (const auto& c: constraints_)
    {
        Antares::logs.error() << c->infeasibility();
    }
}

void InfeasibleProblemReport::logInfeasibilityCauses()
{
    Antares::logs.error() << "Possible causes of infeasibility:";
    for (const auto& c: uniqueConstraintByType_)
    {
        Antares::logs.error() << c->infeasibilityCause();
    }
}

} // namespace Antares::Optimization
