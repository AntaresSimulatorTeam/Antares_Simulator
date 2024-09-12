
#include "OneVarFiller.h"

namespace Antares::Solver::Modeler::Api
{

void OneVarFiller::addVariables()
{
    linearProblem_->addNumVariable(0, 1, added_var_name_);
}

void OneVarFiller::addConstraints()
{
}

void OneVarFiller::addObjective()
{
    auto* var = linearProblem_->getVariable(added_var_name_);
    linearProblem_->setObjectiveCoefficient(var, 1);
}

} // namespace Antares::Solver::Modeler::Api
