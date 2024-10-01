#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneVarFiller: public LinearProblemFiller
{
public:
    explicit OneVarFiller() = default;
    void addVariables(ILinearProblem& pb, LinearProblemData& data) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data) override;

private:
    std::string added_var_name_ = "var-by-OneVarFiller";
};

void OneVarFiller::addVariables(ILinearProblem& pb, LinearProblemData& data)
{
    pb.addNumVariable(0, 1, added_var_name_);
}

void OneVarFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data)
{
}

void OneVarFiller::addObjective(ILinearProblem& pb, LinearProblemData& data)
{
    auto* var = pb.getVariable(added_var_name_);
    pb.setObjectiveCoefficient(var, 1);
}

} // namespace Antares::Solver::Modeler::Api
