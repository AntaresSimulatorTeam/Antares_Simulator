#pragma once
#if 0
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class OneVarFiller: public LinearProblemFiller
{
public:
    explicit OneVarFiller() = default;
    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjective(const FillContext& ctx) override;

private:
    std::string added_var_name_ = "var-by-OneVarFiller";
};

void OneVarFiller::addVariables([[maybe_unused]] const FillContext& ctx)
{
    pb.addNumVariable(0, 1, added_var_name_);
}

void OneVarFiller::addConstraints([[maybe_unused]] const FillContext& ctx)
{
}

void OneVarFiller::addObjective([[maybe_unused]] const FillContext& ctx)
{
    auto* var = pb.lookupVariable(added_var_name_);
    pb.setObjectiveCoefficient(var, 1);
}

} // namespace Antares::Optimisation::LinearProblemApi
#endif