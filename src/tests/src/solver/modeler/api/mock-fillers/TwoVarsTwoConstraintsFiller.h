#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class TwoVarsTwoConstraintsFiller: public LinearProblemFiller
{
public:
    explicit TwoVarsTwoConstraintsFiller() = default;
    void addVariables(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
};

void TwoVarsTwoConstraintsFiller::addVariables(ILinearProblem& pb,
                                               [[maybe_unused]] ILinearProblemData& data,
                                               [[maybe_unused]] FillContext& ctx)
{
    pb.addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraintsFiller");
    pb.addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addConstraints(ILinearProblem& pb,
                                                 [[maybe_unused]] ILinearProblemData& data,
                                                 [[maybe_unused]] FillContext& ctx)
{
    pb.addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraintsFiller");
    pb.addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addObjective([[maybe_unused]] ILinearProblem& pb,
                                               [[maybe_unused]] ILinearProblemData& data,
                                               [[maybe_unused]] FillContext& ctx)
{
}

} // namespace Antares::Solver::Modeler::Api
