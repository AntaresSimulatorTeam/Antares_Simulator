#pragma once

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class OneConstraintFiller final: public LinearProblemFiller
{
public:
    explicit OneConstraintFiller(OptimEntityContainer& optimEntityContainer):
        optimEntityContainer_(optimEntityContainer)
    {
    }

    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjectives(const FillContext& ctx) override;
    void updateUpperBound(unsigned int index, double newUp) override;
    OptimEntityContainer& optimEntityContainer_;
};

void OneConstraintFiller::addVariables([[maybe_unused]] const FillContext& ctx)
{
}

void OneConstraintFiller::addConstraints([[maybe_unused]] const FillContext& ctx)
{
    optimEntityContainer_.Problem().addConstraint(1, 2, "constraint-by-OneConstraintFiller");
}

void OneConstraintFiller::addObjectives([[maybe_unused]] const FillContext& ctx)
{
}

inline void OneConstraintFiller::updateUpperBound(unsigned int index, double newUp)
{
    // keep empty
}

} // namespace Antares::Optimisation::LinearProblemApi
