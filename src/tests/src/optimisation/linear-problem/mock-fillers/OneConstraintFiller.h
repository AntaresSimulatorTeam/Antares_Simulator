#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class OneConstraintFiller: public LinearProblemFiller
{
public:
    explicit OneConstraintFiller() = default;
    void addVariables(ILinearProblem& pb,
                      ILinearProblemData& data,
                      const FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb,
                        ILinearProblemData& data,
                        const FillContext& ctx) override;
    void addObjective(ILinearProblem& pb,
                      ILinearProblemData& data,
                      const FillContext& ctx) override;
};

void OneConstraintFiller::addVariables([[maybe_unused]] ILinearProblem& pb,
                                       [[maybe_unused]] ILinearProblemData& data,
                                       [[maybe_unused]] const FillContext& ctx)
{
}

void OneConstraintFiller::addConstraints(ILinearProblem& pb,
                                         [[maybe_unused]] ILinearProblemData& data,
                                         [[maybe_unused]] const FillContext& ctx)
{
    pb.addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjective([[maybe_unused]] ILinearProblem& pb,
                                       [[maybe_unused]] ILinearProblemData& data,
                                       [[maybe_unused]] const FillContext& ctx)
{
}

} // namespace Antares::Optimisation::LinearProblemApi
