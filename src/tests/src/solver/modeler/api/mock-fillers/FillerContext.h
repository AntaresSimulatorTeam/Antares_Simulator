#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class VarFillerContext: public LinearProblemFiller
{
public:
    explicit VarFillerContext() = default;
    void addVariables(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx) override;

    std::array<int, 3> timeseries = {1, 3, 5};
};

void VarFillerContext::addVariables(ILinearProblem& pb,
                                    [[maybe_unused]] LinearProblemData& data,
                                    [[maybe_unused]] FillContext& ctx)
{
    for (unsigned timestep = ctx.getFirstTimeStep(); timestep < ctx.getLastTimeStep(); timestep++)
    {
        for (unsigned scenario: ctx.scenariosSelected)
        {
            pb.addNumVariable(timeseries[scenario],
                              timeseries[scenario],
                              std::to_string(scenario) + "-" + std::to_string(timestep));
        }
    }
}

void VarFillerContext::addConstraints([[maybe_unused]] ILinearProblem& pb,
                                      [[maybe_unused]] LinearProblemData& data,
                                      [[maybe_unused]] FillContext& ctx)
{
}

void VarFillerContext::addObjective([[maybe_unused]] ILinearProblem& pb,
                                    [[maybe_unused]] LinearProblemData& data,
                                    [[maybe_unused]] FillContext& ctx)
{
}

} // namespace Antares::Solver::Modeler::Api
