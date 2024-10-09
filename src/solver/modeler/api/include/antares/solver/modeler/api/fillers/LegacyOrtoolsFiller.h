#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{
class LegacyOrtoolsFiller: public LinearProblemFiller
{
public:
    explicit LegacyOrtoolsFiller() = default;
    void addVariables(ILinearProblem& pb, LinearProblemData& data) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data) override;
};

void LegacyOrtoolsFiller::addVariables(ILinearProblem& pb, LinearProblemData& data)
{
}

void LegacyOrtoolsFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data)
{
}

void LegacyOrtoolsFiller::addObjective(ILinearProblem& pb, LinearProblemData& data)
{
}
}
