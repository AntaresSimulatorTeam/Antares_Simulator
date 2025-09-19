#pragma once
#if 0
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class TwoVarsTwoConstraintsFiller: public LinearProblemFiller
{
public:
    explicit TwoVarsTwoConstraintsFiller() = default;
    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjective(const FillContext& ctx) override;
};

void TwoVarsTwoConstraintsFiller::addVariables([[maybe_unused]] const FillContext& ctx)
{
    pb.addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraintsFiller");
    pb.addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addConstraints([[maybe_unused]] const FillContext& ctx)
{
    pb.addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraintsFiller");
    pb.addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraintsFiller");
}

void TwoVarsTwoConstraintsFiller::addObjective([[maybe_unused]] const FillContext& ctx)
{
}

} // namespace Antares::Optimisation::LinearProblemApi
#endif
