
#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class TwoVarsTwoConstraintsFiller final: public LinearProblemFiller
{
public:
    explicit TwoVarsTwoConstraintsFiller(OptimEntityContainer& optimEntityContainer):
        optimEntityContainer_(optimEntityContainer)
    {
    }

    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjectives(const FillContext& ctx) override;
    OptimEntityContainer& optimEntityContainer_;
};

void TwoVarsTwoConstraintsFiller::addVariables([[maybe_unused]] const FillContext& ctx)
{
    optimEntityContainer_.Problem().addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraintsFiller");
    optimEntityContainer_.Problem().addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addConstraints([[maybe_unused]] const FillContext& ctx)
{
    optimEntityContainer_.Problem().addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraintsFiller");
    optimEntityContainer_.Problem().addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addObjectives([[maybe_unused]] const FillContext& ctx)
{
}

} // namespace Antares::Optimisation::LinearProblemApi
