#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneVarFiller: public LinearProblemFiller
{
    using LinearProblemFiller::LinearProblemFiller;

public:
    void addVariables() override;
    void addConstraints() override;
    void addObjective() override;

private:
    std::string added_var_name_ = "var-by-OneVarFiller";
};

void OneVarFiller::addVariables()
{
    linearProblem_.addNumVariable(0, 1, added_var_name_);
}

void OneVarFiller::addConstraints()
{
}

void OneVarFiller::addObjective()
{
    auto* var = linearProblem_.getVariable(added_var_name_);
    linearProblem_.setObjectiveCoefficient(var, 1);
}

} // namespace Antares::Solver::Modeler::Api
