// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/Modeler.h"

#include <fstream>
#include <stdexcept>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/StructuredLinearProblem.h>
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/solver/modeler/parameters/parseModelerParameters.h>
#include <antares/solver/optim-model-filler/ComponentFiller.h>
#include "antares/io/outputs/MPSGenerator.h"
#include "antares/solver/modeler/ILoader.h"
#include "antares/solver/modeler/IWriter.h"
#include "antares/utils/utils.h"

using namespace Antares;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares::Optimization;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Solver
{
Modeler::Modeler(ILoader& loader, IWriter& writer):
    loader_{loader},
    writer_{writer}
{
    parameters_ = loader_.loadParameters();
    logs.info() << "Parameters loaded";
    auto data = loader_.loadAll();
    if (!data.has_value())
    {
        throw ModelerError("Error while loading files, exiting");
    }
    // Move the loaded ModelerData out of the optional to avoid copying
    // (ModelerData contains unique_ptr members and is move-only).
    data_ = std::move(*data);

    timeScenarioCtx_ = std::make_unique<FillContext>(
      parameters_.firstTimeStep,
      parameters_.lastTimeStep,
      parameters_.firstTimeStep, // global = local, single time block in pure modeler (for now)
      parameters_.lastTimeStep,  // global = local
      0);
}

class SystemLinearProblemBuilder final
{
public:
    explicit SystemLinearProblemBuilder(const ModelerStudy::SystemModel::System* system,
                                        const ILinearProblemData* data,
                                        const ScenarioGroupRepository& scenarioGroupRepository,
                                        BendersDecomposition* bendersDecomposition,
                                        OptimEntityContainer& optimEntityContainer):
        system_(system),
        data_(data),
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
                                                        data_,
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
    const ILinearProblemData* data_;
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

std::unique_ptr<ILinearProblem> getProblem(bool isMip,
                                           const ResolutionMode& resolutionMode,
                                           const std::optional<std::string>& solver)
{
    if (resolutionMode == ResolutionMode::SEQUENTIAL_SUBPROBLEMS)
    {
        if (!solver)
        {
            throw std::invalid_argument(
              "Please provide a solver for sequential subproblem resolution");
        }
        return std::make_unique<OrtoolsLinearProblem>(isMip, solver.value());
    }
    return std::make_unique<StructuredLinearProblem>();
}

ProblemEntity buildProblem(const ModelerData& data,
                           const Config::Location& location,
                           const std::string& problemId,
                           BendersDecomposition* bendersDecomposition,
                           const FillContext& timeScenarioCtx,
                           const ResolutionMode& resolutionMode,
                           const std::optional<std::string>& solver)
{
    auto [hasCompatibleVariable, isMip] = analyzeLocation(data, location);
    if (!hasCompatibleVariable)
    {
        return {nullptr, nullptr};
    }
    auto problem = getProblem(isMip, resolutionMode, solver);
    auto optimEntityContainer = std::make_unique<OptimEntityContainer>(*problem);

    SystemLinearProblemBuilder builder(data.system.get(),
                                       data.dataSeries.get(),
                                       data.scenarioGroupRepository,
                                       bendersDecomposition,
                                       *optimEntityContainer);

    bendersDecomposition->setCurrentProblemId(problemId);
    builder.build(timeScenarioCtx, location);
    return {std::move(problem), (std::move(optimEntityContainer))};
}

IMipSolution* Modeler::solveSubproblem()
{
    Utils::TimeMeasurement measure;
    logs.info() << "Launching resolution...";
    measure.reset();
    auto& subproblem_1_1 = subproblems_[0];
    auto* solution = subproblem_1_1->solve(parameters_.solverLogs);
    measure.tick();
    logs.info() << "Solved in " << measure.toStringInSeconds();
    return solution;
}

void Modeler::writeSubProblemSimulationTable(const IMipSolution* solution,
                                             const FillContext& timeScenarioCtx) const
{
    switch (solution->getStatus())
    {
    case MipStatus::OPTIMAL:
    case MipStatus::FEASIBLE:
    {
        if (!parameters_.noOutput)
        {
            auto& subproblem_1_1 = subproblems_[0];
            writer_.writeSimulationTable(*subproblem_1_1,
                                         *solution,
                                         data_,
                                         *subproblemOptimEntityContainer_,
                                         timeScenarioCtx);
        }
    }
    break;
    default:
        logs.error() << "Problem during linear optimization";
    }
}

void Modeler::exportMps() const
{
    const auto& output = writer_.outputPath();

    // 1-1.mps
    if (auto& subproblem_1_1 = subproblems_[0])
    {
        const auto mps = IO::Outputs::MPSGenerator(*subproblem_1_1, "1-1").run();
        Antares::IO::Outputs::MPSFileWriter::write(output / "1-1.mps", mps);
    }
    // master.mps
    if (masterProblem_)
    {
        const auto mps = IO::Outputs::MPSGenerator(*masterProblem_, "master").run();
        Antares::IO::Outputs::MPSFileWriter::write(output / "master.mps", mps);
    }
}

void Modeler::exportStructureFile() const
{
    const auto& output = writer_.outputPath();

    // structure.txt
    const BendersDecompositionWriter writer(data_.bendersDecomposition);
    std::ofstream of(output / "structure.txt");
    writer.write(of);
}

void Modeler::buildProblems()
{
    Utils::TimeMeasurement measure;

    logs.info() << "linear problem of System loaded";

    buildMasterProblem();
    buildSubProblem();

    logs.info() << "Linear problem provided";

    auto& subproblem_1_1 = subproblems_[0];
    logs.info() << "Number of variables: " << subproblem_1_1->variableCount();
    logs.info() << "Number of constraints: " << subproblem_1_1->constraintCount();

    measure.tick();
    logs.info();
    logs.info() << "Modeler build took " << measure.toStringInSeconds();
}

void Modeler::buildMasterProblem()
{
    auto masterEntities = buildProblem(data_,
                                       Config::Location::MASTER,
                                       "master",
                                       &data_.bendersDecomposition,
                                       *timeScenarioCtx_,
                                       ResolutionMode::BENDERS_DECOMPOSITION,
                                       std::nullopt);
    masterProblem_ = std::move(masterEntities.problem);
}

void Modeler::buildSubProblem()
{
    auto [subproblem, subproblemOptimEntityContainer] = buildProblem(data_,
                                                                     Config::Location::SUBPROBLEMS,
                                                                     "1-1",
                                                                     &data_.bendersDecomposition,
                                                                     *timeScenarioCtx_,
                                                                     data_.resolutionMode,
                                                                     parameters_.solver);
    subproblems_.emplace_back(std::move(subproblem));
    subproblemOptimEntityContainer_ = std::move(subproblemOptimEntityContainer);
}

void Modeler::run()
{
    buildProblems();
    // if simulation table or mps are requested
    if (!parameters_.noOutput || parameters_.exportMps)
    {
        const auto simulationTableSuffix = formatTime(getCurrentTime(), "%Y%m%d-%H%M");
        writer_.init(simulationTableSuffix);
    }
    if (parameters_.exportMps)
    {
        exportMps();
        exportStructureFile();
    }
    if (data_.resolutionMode == ResolutionMode::SEQUENTIAL_SUBPROBLEMS)
    {
        auto* solution = solveSubproblem();
        writeSubProblemSimulationTable(solution, *timeScenarioCtx_);
    }
}

} // namespace Antares::Solver
