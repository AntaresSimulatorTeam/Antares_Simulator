#pragma once

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class OneVarFiller final: public LinearProblemFiller
{
public:
    explicit OneVarFiller(OptimEntityContainer& optimEntityContainer):
        optimEntityContainer_(optimEntityContainer)
    {
    }

    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjectives(const FillContext& ctx) override;
    void updateUpperBound(unsigned int index, double newUp) override;

private:
    std::string added_var_name_ = "var-by-OneVarFiller";
    OptimEntityContainer& optimEntityContainer_;
};

void OneVarFiller::addVariables([[maybe_unused]] const FillContext& ctx)
{
    optimEntityContainer_.Problem().addNumVariable(0, 1, added_var_name_);
}

void OneVarFiller::addConstraints([[maybe_unused]] const FillContext& ctx)
{
}

void OneVarFiller::addObjectives([[maybe_unused]] const FillContext& ctx)
{
    auto* var = optimEntityContainer_.Problem().lookupVariable(added_var_name_);
    optimEntityContainer_.Problem().setObjectiveCoefficient(var, 1);
}

inline void OneVarFiller::updateUpperBound(unsigned int index, double newUp)
{
    // keep empty
}

} // namespace Antares::Optimisation::LinearProblemApi
