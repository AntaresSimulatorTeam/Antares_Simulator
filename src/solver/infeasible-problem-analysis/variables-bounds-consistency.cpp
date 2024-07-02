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
#include "antares/solver/infeasible-problem-analysis/variables-bounds-consistency.h"

#include <antares/logs/logs.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

using namespace operations_research;

namespace Antares::Optimization
{

void VariablesBoundsConsistency::run(MPSolver* problem)
{
    for (const auto& var: problem->variables())
    {
        double lowBound = var->lb();
        double upBound = var->ub();
        std::string name = var->name();
        if (lowBound > upBound)
        {
            storeIncorrectVariable(name, lowBound, upBound);
        }
    }

    if (foundIncorrectVariables())
    {
        hasDetectedInfeasibilityCause_ = true;
    }
}

void VariablesBoundsConsistency::storeIncorrectVariable(std::string name,
                                                        double lowBound,
                                                        double upBound)
{
    incorrectVars_.push_back(VariableBounds(name, lowBound, upBound));
}

bool VariablesBoundsConsistency::foundIncorrectVariables()
{
    return !incorrectVars_.empty();
}

void VariablesBoundsConsistency::printReport() const
{
    for (const auto& var: incorrectVars_)
    {
        logs.notice() << var.name << " : low bound = " << var.lowBound
                      << ", up bound = " << var.upBound;
    }
}
} // namespace Antares::Optimization
