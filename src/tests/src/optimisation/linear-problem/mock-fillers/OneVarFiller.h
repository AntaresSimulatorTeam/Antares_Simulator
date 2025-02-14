#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class OneVarFiller: public LinearProblemFiller
{
public:
    explicit OneVarFiller() = default;
    void addVariables(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;

private:
    std::string added_var_name_ = "var-by-OneVarFiller";
};

void OneVarFiller::addVariables(ILinearProblem& pb,
                                [[maybe_unused]] ILinearProblemData& data,
                                [[maybe_unused]] FillContext& ctx)
{
    pb.addNumVariable(0, 1, added_var_name_);
}

void OneVarFiller::addConstraints([[maybe_unused]] ILinearProblem& pb,
                                  [[maybe_unused]] ILinearProblemData& data,
                                  [[maybe_unused]] FillContext& ctx)
{
}

void OneVarFiller::addObjective(ILinearProblem& pb,
                                [[maybe_unused]] ILinearProblemData& data,
                                [[maybe_unused]] FillContext& ctx)
{
    auto* var = pb.getVariable(added_var_name_);
    pb.setObjectiveCoefficient(var, 1);
}

} // namespace Antares::Optimisation::LinearProblemApi
