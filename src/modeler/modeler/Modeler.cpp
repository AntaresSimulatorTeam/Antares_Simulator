// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/Modeler.h"

#include <fstream>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include <antares/solver/modeler/parameters/parseModelerParameters.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include "antares/io/outputs/MPSGenerator.h"
#include "antares/solver/modeler/ILoader.h"
#include "antares/solver/modeler/IWriter.h"
#include "antares/utils/utils.h"

using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares;
using namespace Antares::Optimization;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Solver
{
Modeler::Modeler(ILoader& loader, IWriter& writer):
    loader_{loader},
    writer_{writer}
{
    try
    {
        parameters_ = loader_.loadParameters();
        logs.info() << "Parameters loaded";
        data_ = loader_.loadAll();
    }
    catch (const LoadFiles::ErrorLoadingYaml&)
    {
        throw ModelerError("Error while loading files, exiting");
    }
}

class SystemLinearProblemBuilder final
{
public:
    explicit SystemLinearProblemBuilder(const ModelerStudy::SystemModel::System* system,
                                        const ScenarioGroupRepository& scenarioGroupRepository,
                                        BendersDecomposition* bendersDecomposition,
                                        OptimEntityContainer& optimEntityContainer):
        system_(system),
        scenarioGroupRepository_(scenarioGroupRepository),
        bendersDecomposition_(bendersDecomposition),
        optimEntityContainer_(optimEntityContainer)
    {
    }

    ~SystemLinearProblemBuilder() = default;

    void build(const FillContext& timeScenarioCtx, Config::Location location)
    {
        std::vector<std::unique_ptr<LinearProblemFiller>> fillers;
        const auto& components = system_->Components();
        optimEntityContainer_.addFromSystemComponents(components, location);

        for (const auto& component: components)
        {
            auto cf = std::make_unique<ComponentFiller>(component,
                                                        optimEntityContainer_,
                                                        scenarioGroupRepository_,
                                                        location,
                                                        bendersDecomposition_);
            fillers.push_back(std::move(cf));
        }

        LinearProblemBuilder linear_problem_builder(fillers);
        linear_problem_builder.build(timeScenarioCtx);
    }

private:
    const ModelerStudy::SystemModel::System* system_;
    const ScenarioGroupRepository& scenarioGroupRepository_;
    BendersDecomposition* bendersDecomposition_ = nullptr;
    OptimEntityContainer& optimEntityContainer_;
};

struct LocationAnalysis
{
    bool hasCompatibleVariable = false;
    bool isMip = false;
};

LocationAnalysis analyzeLocation(const ModelerData& data, const Config::Location& location)
{
    LocationAnalysis result;

    for (const auto& component: data.system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            if (!AreLocationsCompatibleForFillers(location, variable.location()))
            {
                continue;
            }

            result.hasCompatibleVariable = true;

            if (variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT)
            {
                result.isMip = true;
                return result;
            }
        }
    }

    return result;
}

struct ProblemEntity
{
    std::unique_ptr<ILinearProblem> problem;
    std::unique_ptr<OptimEntityContainer> optimEntityContainer;
};

ProblemEntity buildProblem(const Antares::Solver::ModelerData& data,
                           const Config::Location& location,
                           const std::string& problemId,
                           BendersDecomposition* bendersDecomposition,
                           const FillContext& timeScenarioCtx,
                           const std::string& solver)
{
    auto [hasCompatibleVariable, isMip] = analyzeLocation(data, location);
    if (!hasCompatibleVariable)
    {
        return {nullptr, nullptr};
    }
    auto problem = std::make_unique<OrtoolsLinearProblem>(isMip, solver);
    auto optimEntityContainer = std::make_unique<OptimEntityContainer>(
      *problem,
      data.dataSeries.get(),
      &data.scenarioGroupRepository);
    SystemLinearProblemBuilder builder(data.system.get(),
                                       data.scenarioGroupRepository,
                                       bendersDecomposition,
                                       *optimEntityContainer);

    bendersDecomposition->setCurrentProblemId(problemId);
    builder.build(timeScenarioCtx, location);
    return {std::move(problem), (std::move(optimEntityContainer))};
}

void Modeler::run()
{
    Utils::TimeMeasurement measure;

    logs.info() << "linear problem of System loaded";
    // Problem is MIP if any variable of any component is not continuous

    // Todo: scenario
    FillContext timeScenarioCtx = {
      parameters_.firstTimeStep,
      parameters_.lastTimeStep,
      parameters_.firstTimeStep, // global = local, single time block in pure modeler (for now)
      parameters_.lastTimeStep,  // global = local
      0};

    // Sub problem
    BendersDecomposition bendersDecomposition;

    // Master

    auto masterEntities = buildProblem(data_,
                                       Config::Location::MASTER,
                                       "master",
                                       &bendersDecomposition,
                                       timeScenarioCtx,
                                       parameters_.solver);
    masterProblem_ = std::move(masterEntities.problem);
    // Subproblem
    auto [subproblem, subproblemOptimEntityContainer] = buildProblem(data_,
                                                                     Config::Location::SUBPROBLEMS,
                                                                     "1-1",
                                                                     &bendersDecomposition,
                                                                     timeScenarioCtx,
                                                                     parameters_.solver);
    subproblems_.emplace_back(std::move(subproblem));

    auto& subproblem_1_1 = subproblems_[0];
    // gp : class SystemLinearProblemBuilder should be renamed into ComponentFillersBuilder
    // gp : and build() should return the vector of component fillers
    // Subproblem

    logs.info() << "Linear problem provided";

    logs.info() << "Number of variables: " << subproblem_1_1->variableCount();
    logs.info() << "Number of constraints: " << subproblem_1_1->constraintCount();

    measure.tick();
    logs.info();
    logs.info() << "Modeler build took " << measure.toStringInSeconds();

    // if simulation table or mps are requested
    if (!parameters_.noOutput || parameters_.exportMps)
    {
        const auto simulationTableSuffix = formatTime(getCurrentTime(), "%Y%m%d-%H%M");
        writer_.init(simulationTableSuffix);
    }
    if (parameters_.exportMps)
    {
        auto output = writer_.outputPath();

        // 1-1.mps
        if (subproblem_1_1)
        {
            auto mps = IO::Outputs::MPSGenerator(*subproblem_1_1, "1-1").run();
            Antares::IO::Outputs::MPSFileWriter::write(output / "1-1.mps", mps);
        }
        // master.mps
        if (masterProblem_)
        {
            auto mps = IO::Outputs::MPSGenerator(*masterProblem_, "master").run();
            Antares::IO::Outputs::MPSFileWriter::write(output / "master.mps", mps);
        }
        // structure.txt
        BendersDecompositionWriter writer(bendersDecomposition);
        std::ofstream of(output / "structure.txt");
        writer.write(of);
    }

    logs.info() << "Launching resolution...";
    measure.reset();
    auto* solution = subproblem_1_1->solve(parameters_.solverLogs);
    measure.tick();
    logs.info() << "Solved in " << measure.toStringInSeconds();

    switch (solution->getStatus())
    {
    case MipStatus::OPTIMAL:
    case MipStatus::FEASIBLE:
    {
        if (!parameters_.noOutput)
        {
            writer_.writeSimulationTable(*subproblem_1_1,
                                         *solution,
                                         data_,
                                         *subproblemOptimEntityContainer,
                                         timeScenarioCtx);
        }
    }
    break;
    default:
        logs.error() << "Problem during linear optimization";
    }
}
} // namespace Antares::Solver
