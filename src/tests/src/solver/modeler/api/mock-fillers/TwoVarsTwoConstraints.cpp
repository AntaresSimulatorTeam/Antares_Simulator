#include "TwoVarsTwoConstraints.h"

namespace Antares::Solver::Modeler::Api
{

void TwoVarsTwoConstraints::addVariables()
{
    linearProblem_->addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraints");
    linearProblem_->addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraints");
}

void TwoVarsTwoConstraints::addConstraints()
{
    linearProblem_->addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraints");
    linearProblem_->addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraints");
}

void TwoVarsTwoConstraints::addObjective()
{
}

} // namespace Antares::Solver::Modeler::Api
