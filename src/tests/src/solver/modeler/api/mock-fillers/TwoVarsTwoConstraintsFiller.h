#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class TwoVarsTwoConstraintsFiller: public LinearProblemFiller
{
    using LinearProblemFiller::LinearProblemFiller;

public:
    void addVariables() override;
    void addConstraints() override;
    void addObjective() override;
};

void TwoVarsTwoConstraintsFiller::addVariables()
{
    linearProblem_->addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraintsFiller");
    linearProblem_->addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addConstraints()
{
    linearProblem_->addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraintsFiller");
    linearProblem_->addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addObjective()
{
}

} // namespace Antares::Solver::Modeler::Api
