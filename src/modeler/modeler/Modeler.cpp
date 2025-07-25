
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

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include <antares/solver/modeler/parameters/parseModelerParameters.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include "antares/solver/modeler/ILoader.h"
#include "antares/solver/modeler/IWriter.h"

using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares;
using namespace Antares::Optimization;
using namespace Antares::Solver;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Solver
{
Modeler::Modeler(ILoader& loader, IWriter& writer):
    loader_{loader},
    writer_{writer}
{
}

class SystemLinearProblemBuilder
{
public:
    explicit SystemLinearProblemBuilder(const ModelerStudy::SystemModel::System* system):
        system_(system)
    {
    }

    ~SystemLinearProblemBuilder() = default;

    void Provide(ILinearProblem& pb,
                 const ModelerParameters& parameters,
                 ILinearProblemData* dataSeries,
                 const Optimisation::ScenarioGroupRepository& scenario_group_repository)
    {
        std::vector<std::unique_ptr<Optimisation::ComponentFiller>> fillers;
        std::vector<LinearProblemFiller*> fillers_ptr;
        // All LP variables coordinates (component id, variable id, scenario, time step)
        VariableDictionary variableDictionary;

        for (const auto& [_, component]: system_->Components())
        {
            auto cf = std::make_unique<Optimisation::ComponentFiller>(component,
                                                                      variableDictionary,
                                                                      scenario_group_repository);
            fillers.push_back(std::move(cf));
        }
        for (auto& component_filler: fillers)
        {
            fillers_ptr.push_back(component_filler.get());
        }

        LinearProblemBuilder linear_problem_builder(fillers_ptr);
        // Todo: scenario
        FillContext time_scenario_ctx = {parameters.firstTimeStep, parameters.lastTimeStep, 0};
        linear_problem_builder.build(pb, *dataSeries, time_scenario_ctx);
    }

private:
    const ModelerStudy::SystemModel::System* system_;
};

void Modeler::solve() const
{
    try
    {
        const auto parameters = loader_.loadParameters();
        logs.info() << "Parameters loaded";
        const auto data = loader_.loadAll();

        SystemLinearProblemBuilder system_linear_problem(data.system.get());

        writer_.init(!parameters.noOutput);

        logs.info() << "linear problem of System loaded";
        // Problem is MIP if any variable of any component is not continuous
        bool isMip = std::ranges::any_of(
          data.system->Components() | std::views::values,
          [](const auto& component)
          {
              return std::ranges::any_of(component.getModel()->Variables() | std::views::values,
                                         [](const auto& variable) {
                                             return variable.Type()
                                                    != ModelerStudy::SystemModel::ValueType::FLOAT;
                                         });
          });
        OrtoolsLinearProblem ortools_linear_problem(isMip, parameters.solver);

        system_linear_problem.Provide(ortools_linear_problem,
                                      parameters,
                                      data.dataSeries.get(),
                                      data.scenario_group_repository);

        logs.info() << "Linear problem provided";

        logs.info() << "Number of variables: " << ortools_linear_problem.variableCount();
        logs.info() << "Number of constraints: " << ortools_linear_problem.constraintCount();

        writer_.writeProblem(ortools_linear_problem);

        logs.info() << "Launching resolution...";
        auto* solution = ortools_linear_problem.solve(parameters.solverLogs);
        switch (solution->getStatus())
        {
        case MipStatus::OPTIMAL:
        case MipStatus::FEASIBLE:
            writer_.writeSolution(*solution);
            break;
        default:
            logs.error() << "Problem during linear optimization";
        }
    }
    catch (const LoadFiles::ErrorLoadingYaml&)
    {
        throw Antares::Solver::Modeler::ModelerError("Error while loading files, exiting");
    }
}
} // namespace Antares::Solver
