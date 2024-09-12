#include "OneConstraintFiller.h"

namespace Antares::Solver::Modeler::Api
{

void OneConstraintFiller::addVariables()
{
}

void OneConstraintFiller::addConstraints()
{
    linearProblem_->addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjective()
{
}

} // namespace Antares::Solver::Modeler::Api
