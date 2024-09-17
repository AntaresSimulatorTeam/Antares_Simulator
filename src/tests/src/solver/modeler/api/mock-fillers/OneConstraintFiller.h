#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class OneConstraintFiller: public LinearProblemFiller
{
public:
    explicit OneConstraintFiller() = default;
    void addVariables(ILinearProblem& pb, LinearProblemData& data) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data) override;
};

void OneConstraintFiller::addVariables(ILinearProblem& pb, LinearProblemData& data)
{
}

void OneConstraintFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data)
{
    pb.addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjective(ILinearProblem& pb, LinearProblemData& data)
{
}

} // namespace Antares::Solver::Modeler::Api
