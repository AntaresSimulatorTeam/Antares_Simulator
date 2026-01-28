// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
