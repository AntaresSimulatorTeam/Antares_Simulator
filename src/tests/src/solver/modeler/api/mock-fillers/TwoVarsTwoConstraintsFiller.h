#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class TwoVarsTwoConstraintsFiller: public LinearProblemFiller
{
public:
    explicit TwoVarsTwoConstraintsFiller() = default;
    void addVariables(ILinearProblem& pb, LinearProblemData& data) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data) override;
};

void TwoVarsTwoConstraintsFiller::addVariables(ILinearProblem& pb, LinearProblemData& data)
{
    pb.addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraintsFiller");
    pb.addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data)
{
    pb.addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraintsFiller");
    pb.addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addObjective(ILinearProblem& pb, LinearProblemData& data)
{
}

} // namespace Antares::Solver::Modeler::Api
