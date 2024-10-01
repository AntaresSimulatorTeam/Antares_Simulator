#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneConstraintFiller: public LinearProblemFiller
{
public:
    explicit OneConstraintFiller() = default;
    void addVariables(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx) override;
};

void OneConstraintFiller::addVariables(ILinearProblem& pb,
                                       LinearProblemData& data,
                                       FillContext& ctx)
{
}

void OneConstraintFiller::addConstraints(ILinearProblem& pb,
                                         LinearProblemData& data,
                                         FillContext& ctx)
{
    pb.addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjective(ILinearProblem& pb,
                                       LinearProblemData& data,
                                       FillContext& ctx)
{
}

} // namespace Antares::Solver::Modeler::Api
