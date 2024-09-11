
#include "LinearProblemFillerMocks.h"

namespace Antares::Solver::Modeler::Api
{

void OneVarFiller::addVariables()
{
    linearProblem_->addNumVariable(0, 1, "var");
}

void OneVarFiller::addConstraints()
{
}

void OneVarFiller::addObjective()
{
}

}