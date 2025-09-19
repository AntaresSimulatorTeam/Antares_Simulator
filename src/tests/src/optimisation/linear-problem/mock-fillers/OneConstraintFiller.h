#pragma once
#if 0
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class OneConstraintFiller: public LinearProblemFiller
{
public:
    explicit OneConstraintFiller() = default;
    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjective(const FillContext& ctx) override;
};

void OneConstraintFiller::addVariables([[maybe_unused]] const FillContext& ctx)
{
}

void OneConstraintFiller::addConstraints([[maybe_unused]] const FillContext& ctx)
{
    pb.addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjective([[maybe_unused]] const FillContext& ctx)
{
}

} // namespace Antares::Optimisation::LinearProblemApi
#endif
