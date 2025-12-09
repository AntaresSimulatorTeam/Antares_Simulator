
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
      const Optimisation::ScenarioGroupRepository& scenarioGroupRepository,
      BendersDecomposition* bendersDecomposition):
        system_(system),
        scenarioGroupRepository_(scenarioGroupRepository),
        optimEntityContainer_(pb, &dataSeries, &scenarioGroupRepository),
        bendersDecomposition_(bendersDecomposition)
    {
    }

    ~SystemLinearProblemBuilder() = default;

    void build(const FillContext& timeScenarioCtx, Antares::Modeler::Config::Location location)
    {
        std::vector<std::unique_ptr<LinearProblemFiller>> fillers;
        const auto& components = system_->Components();
        optimEntityContainer_.addFromSystemComponents(components, location);

        for (const auto& component: components)
        {
            auto cf = std::make_unique<Optimisation::ComponentFiller>(component,
                                                                      optimEntityContainer_,
                                                                      scenarioGroupRepository_,
                                                                      location,
                                                                      bendersDecomposition_);
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
    const Optimisation::ScenarioGroupRepository& scenarioGroupRepository_;
    Optimisation::OptimEntityContainer optimEntityContainer_;
    BendersDecomposition* bendersDecomposition_ = nullptr;
};

void Modeler::run() const
{
    Antares::Solver::ModelerParameters parameters;
    Antares::Modeler::Data data;

    try
    {
        parameters = loader_.loadParameters();
        logs.info() << "Parameters loaded";
        data = loader_.loadAll();
    }
    catch (const LoadFiles::ErrorLoadingYaml&)
    {
        throw ModelerError("Error while loading files, exiting");
    }

    Utils::TimeMeasurement measure;

    logs.info() << "linear problem of System loaded";
    // Problem is MIP if any variable of any component is not continuous
    bool isMip = std::ranges::any_of(
      data.system->Components(),
      [](const auto& component)
      {
          return std::ranges::any_of(component.getModel()->Variables(),
                                     [](const auto& variable) {
                                         return variable.Type()
                                                != ModelerStudy::SystemModel::ValueType::FLOAT;
                                     });
      });

    // Todo: scenario
    FillContext timeScenarioCtx = {
      parameters.firstTimeStep,
      parameters.lastTimeStep,
      parameters.firstTimeStep, // global = local, single time block in pure modeler (for now)
      parameters.lastTimeStep,  // global = local
      0};

    // Sub problem
    BendersDecomposition bendersDecomposition;

    // Master
    OrtoolsLinearProblem master_problem(isMip, parameters.solver);
    SystemLinearProblemBuilder master_builder(data.system.get(),
                                              master_problem,
                                              *data.dataSeries,
                                              data.scenarioGroupRepository,
                                              &bendersDecomposition);

    bendersDecomposition.setCurrentProblemId("master");
    master_builder.build(timeScenarioCtx, Antares::Modeler::Config::Location::MASTER);

    // Subproblem
    OrtoolsLinearProblem subproblem(isMip, parameters.solver);

    // gp : class SystemLinearProblemBuilder should be renamed into ComponentFillersBuilder
    // gp : and build() should return the vector of component fillers
    // Subproblem
    SystemLinearProblemBuilder subproblem_builder(data.system.get(),
                                                  subproblem,
                                                  *data.dataSeries,
                                                  data.scenarioGroupRepository,
                                                  &bendersDecomposition);

    bendersDecomposition.setCurrentProblemId("1-1");
    subproblem_builder.build(timeScenarioCtx, Antares::Modeler::Config::Location::SUBPROBLEMS);

    logs.info() << "Linear problem provided";

    logs.info() << "Number of variables: " << subproblem.variableCount();
    logs.info() << "Number of constraints: " << subproblem.constraintCount();

    measure.tick();
    logs.info();
    logs.info() << "Modeler build took " << measure.toStringInSeconds();

    const auto simulationTableSuffix = formatTime(getCurrentTime(), "%Y%m%d-%H%M");

    if (!parameters.noOutput)
    {
        writer_.init(simulationTableSuffix);
        auto output = writer_.outputPath();

        // 1-1.mps
        Write(subproblem, output / "1-1.mps");
        // master.mps
        Write(master_problem, output / "master.mps");

        // structure.txt
        BendersDecompositionWriter writer(bendersDecomposition);
        std::ofstream of(output / "structure.txt");
        writer.write(of);
    }

    logs.info() << "Launching resolution...";
    measure.reset();
    auto* solution = subproblem.solve(parameters.solverLogs);
    measure.tick();
    logs.info() << "Solved in " << measure.toStringInSeconds();

    switch (solution->getStatus())
    {
    case MipStatus::OPTIMAL:
    case MipStatus::FEASIBLE:
    {
        writer_.writeSimulationTable(subproblem,
                                     *solution,
                                     data,
                                     subproblem_builder.getOptimEntityContainer(),
                                     timeScenarioCtx);
    }
    break;
    default:
        logs.error() << "Problem during linear optimization";
    }
}
} // namespace Antares::Solver
