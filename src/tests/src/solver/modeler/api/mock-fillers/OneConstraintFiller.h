#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneConstraintFiller: public LinearProblemFiller
{
    using LinearProblemFiller::LinearProblemFiller;

public:
    void addVariables() override;
    void addConstraints() override;
    void addObjective() override;
};

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
