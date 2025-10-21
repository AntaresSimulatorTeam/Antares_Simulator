#pragma once

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"

namespace Antares::Optimisation::LinearProblemApi
{

class VarFillerContext final: public LinearProblemFiller
{
public:
    explicit VarFillerContext(OptimEntityContainer& optimEntityContainer):
        optimEntityContainer_(optimEntityContainer)
    {
    }

    void addVariables(const FillContext& ctx) override;
    void addConstraints(const FillContext& ctx) override;
    void addObjective(const FillContext& ctx) override;

    std::array<std::array<int, 3>, 5> timeseries = {
      {{1, 3, 5}, {2, 4, 6}, {7, 9, 11}, {8, 10, 12}, {13, 15, 17}}};
    OptimEntityContainer& optimEntityContainer_;
};

void VarFillerContext::addVariables([[maybe_unused]] const FillContext& ctx)
{
    for (unsigned timestep = ctx.getLocalFirstTimeStep(); timestep < ctx.getLocalLastTimeStep();
         timestep++)
    {
        for (unsigned scenario: ctx.getSelectedScenarios())
        {
            optimEntityContainer_.Problem().addNumVariable(timeseries[timestep][scenario],
                                                           timeseries[timestep][scenario],
                                                           "variable-ts" + std::to_string(timestep)
                                                             + "-sc" + std::to_string(scenario));
        }
    }
}

void VarFillerContext::addConstraints([[maybe_unused]] const FillContext& ctx)
{
}

void VarFillerContext::addObjective([[maybe_unused]] const FillContext& ctx)
{
}

} // namespace Antares::Optimisation::LinearProblemApi
