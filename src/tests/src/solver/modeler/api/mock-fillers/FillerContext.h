#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"

namespace Antares::Solver::Modeler::Api
{

class VarFillerContext: public LinearProblemFiller
{
public:
    explicit VarFillerContext() = default;
    void addVariables(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem& pb, ILinearProblemData& data, FillContext& ctx) override;

    std::array<std::array<int, 3>, 5> timeseries = {
      {{1, 3, 5}, {2, 4, 6}, {7, 9, 11}, {8, 10, 12}, {13, 15, 17}}};
};

void VarFillerContext::addVariables(ILinearProblem& pb,
                                    [[maybe_unused]] ILinearProblemData& data,
                                    [[maybe_unused]] FillContext& ctx)
{
    for (unsigned timestep = ctx.getFirstTimeStep(); timestep < ctx.getLastTimeStep(); timestep++)
    {
        for (unsigned scenario: ctx.scenariosSelected)
        {
            pb.addNumVariable(timeseries[timestep][scenario],
                              timeseries[timestep][scenario],
                              "variable-ts" + std::to_string(timestep) + "-sc"
                                + std::to_string(scenario));
        }
    }
}

void VarFillerContext::addConstraints([[maybe_unused]] ILinearProblem& pb,
                                      [[maybe_unused]] ILinearProblemData& data,
                                      [[maybe_unused]] FillContext& ctx)
{
}

void VarFillerContext::addObjective([[maybe_unused]] ILinearProblem& pb,
                                    [[maybe_unused]] ILinearProblemData& data,
                                    [[maybe_unused]] FillContext& ctx)
{
}

} // namespace Antares::Solver::Modeler::Api
