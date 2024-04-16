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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <map>
#include <string>
#include <vector>

#include "constraint.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

namespace Antares
{
namespace Optimization
{
class InfeasibleProblemReport
{
public:
    InfeasibleProblemReport() = delete;
    explicit InfeasibleProblemReport(
            const std::vector<const operations_research::MPVariable*>& slackVariables);
    void prettyPrint();

private:
    void turnSlackVarsIntoConstraints(
            const std::vector<const operations_research::MPVariable*>& slackVariables);
    void sortConstraints();
    void trimConstraints();
    void extractItems();
    void logSuspiciousConstraints();

    std::vector<Constraint> mConstraints;
    std::map<ConstraintType, unsigned int> mTypes;
    const unsigned int nbVariables = 10;
};
} // namespace Optimization
} // namespace Antares
