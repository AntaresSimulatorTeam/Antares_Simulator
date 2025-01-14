#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneConstraintFiller: public LinearProblemFiller
{
public:
    explicit OneConstraintFiller() = default;
    void addVariables(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
};

void OneConstraintFiller::addVariables([[maybe_unused]] ILinearProblem& pb,
                                       [[maybe_unused]] ILinearProblemData& data,
                                       [[maybe_unused]] FillContext& ctx)
{
}

void OneConstraintFiller::addConstraints(ILinearProblem& pb,
                                         [[maybe_unused]] ILinearProblemData& data,
                                         [[maybe_unused]] FillContext& ctx)
{
    pb.addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjective([[maybe_unused]] ILinearProblem& pb,
                                       [[maybe_unused]] ILinearProblemData& data,
                                       [[maybe_unused]] FillContext& ctx)
{
}

} // namespace Antares::Solver::Modeler::Api
