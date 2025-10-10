
/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/modeler/Modeler.h"

#include <chrono>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include <antares/solver/modeler/parameters/parseModelerParameters.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include "antares/solver/modeler/ILoader.h"
#include "antares/solver/modeler/IWriter.h"
#include "antares/utils/utils.h"

using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares;
using namespace Antares::Optimization;
using namespace Antares::Optimisation;
using namespace Antares::Solver;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Solver
{
Modeler::Modeler(ILoader& loader, IWriter& writer):
    loader_{loader},
    writer_{writer}
{
}

class SystemLinearProblemBuilder final
{
public:
    explicit SystemLinearProblemBuilder(
      const ModelerStudy::SystemModel::System* system,
      ILinearProblem& pb,
      const LinearProblemApi::ILinearProblemData& dataSeries,
      const Optimisation::ScenarioGroupRepository& scenarioGroupRepository):
        system_(system),
        dataSeries_(dataSeries),
        scenarioGroupRepository_(scenarioGroupRepository),
        optimEntityContainer_(pb, &dataSeries, &scenarioGroupRepository)
    {
    }

    ~SystemLinearProblemBuilder() = default;

    void Provide(const FillContext& timeScenarioCtx)
    {
        std::vector<std::unique_ptr<LinearProblemFiller>> fillers;
        const auto& components = system_->Components();
        optimEntityContainer_.addFromSystemComponents(components);

        // All LP variables coordinates (component id, variable id, scenario, time step)
        for (const auto& component: components)
        {
            auto cf = std::make_unique<Optimisation::ComponentFiller>(component,
                                                                      optimEntityContainer_,
                                                                      scenarioGroupRepository_);
            fillers.push_back(std::move(cf));
        }

        LinearProblemBuilder linear_problem_builder(fillers);

        linear_problem_builder.build(timeScenarioCtx);
    }

    [[nodiscard]] const Optimisation::OptimEntityContainer& getOptimEntityContainer() const
    {
        return optimEntityContainer_;
    }

private:
    const ModelerStudy::SystemModel::System* system_;
    const LinearProblemApi::ILinearProblemData& dataSeries_;
    const Optimisation::ScenarioGroupRepository& scenarioGroupRepository_;
    Optimisation::OptimEntityContainer optimEntityContainer_;
};

void Modeler::solve() const
{
    try
    {
        const auto simulationTableSuffix = formatTime(getCurrentTime(), "%Y%m%d-%H%M");
        const auto parameters = loader_.loadParameters();
        logs.info() << "Parameters loaded";
        const auto data = loader_.loadAll();

        Utils::TimeMeasurement measure;

        writer_.init(!parameters.noOutput, simulationTableSuffix);

        logs.info() << "linear problem of System loaded";
        // Problem is MIP if any variable of any component is not continuous
        bool isMip = std::ranges::any_of(
          data.system->Components(),
          [](const auto& component)
          {
              return std::any_of(component.getModel()->Variables().cbegin(),
                                 component.getModel()->Variables().cend(),
                                 [](const auto& variable) {
                                     return variable.Type()
                                            != ModelerStudy::SystemModel::ValueType::FLOAT;
                                 });
          });
        OrtoolsLinearProblem ortools_linear_problem(isMip, parameters.solver);
        // Todo: scenario
        FillContext timeScenarioCtx = {
          parameters.firstTimeStep,
          parameters.lastTimeStep,
          parameters.firstTimeStep, // global = local, single time block in pure modeler (for now)
          parameters.lastTimeStep,  // global = local
          0};
        SystemLinearProblemBuilder system_linear_problem(data.system.get(),
                                                         ortools_linear_problem,
                                                         *data.dataSeries,
                                                         data.scenarioGroupRepository);

        system_linear_problem.Provide(timeScenarioCtx);

        logs.info() << "Linear problem provided";

        logs.info() << "Number of variables: " << ortools_linear_problem.variableCount();
        logs.info() << "Number of constraints: " << ortools_linear_problem.constraintCount();

        measure.tick();
        logs.info();
        logs.info() << "Modeler build took " << measure.toStringInSeconds();

        writer_.writeProblem(ortools_linear_problem);

        logs.info() << "Launching resolution...";
        measure.reset();
        auto* solution = ortools_linear_problem.solve(parameters.solverLogs);
        measure.tick();
        logs.info() << "Solved in " << measure.toStringInSeconds();

        switch (solution->getStatus())
        {
        case MipStatus::OPTIMAL:
        case MipStatus::FEASIBLE:
        {
            writer_.writeSimulationTable(ortools_linear_problem,
                                         *solution,
                                         data,
                                         system_linear_problem.getOptimEntityContainer(),
                                         timeScenarioCtx);
        }
        break;
        default:
            logs.error() << "Problem during linear optimization";
        }
    }
    catch (const LoadFiles::ErrorLoadingYaml&)
    {
        throw ModelerError("Error while loading files, exiting");
    }
}
} // namespace Antares::Solver
