#include "variables-bounds-consistency.h"
#include <antares/logs/logs.h>

using namespace operations_research;

namespace Antares::Optimization
{
    
void VariablesBoundsConsistency::run(MPSolver* problem)
{
    for (const auto& var : problem->variables())
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
        hasDetectedInfeasibilityCause_ = true;
}

void VariablesBoundsConsistency::storeIncorrectVariable(std::string name, double lowBound, double upBound)
{
    incorrectVars_.push_back(VariableBounds(name, lowBound, upBound));
}

bool VariablesBoundsConsistency::foundIncorrectVariables()
{
    return !incorrectVars_.empty();
}

void VariablesBoundsConsistency::printReport() const
{
    for (const auto& var : incorrectVars_)
    {
        logs.notice() << var.name << " : low bound = " << var.lowBound << ", up bound = " << var.upBound;
    }
}
}   // namespace Antares::Optimization