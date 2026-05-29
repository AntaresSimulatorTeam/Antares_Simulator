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
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/solver/modeler/ILoader.h"
#include "antares/utils/utils.h"
#include "antares/writer/table_writer_factory.h"

using namespace Antares;
using namespace Antares::Writer;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;
using namespace Antares::Optimization;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::IO::Outputs;

namespace Antares::Solver
{

bool checkSolution(const IMipSolution* solution)
{
    if (!solution)
    {
        logs.error() << "Solution to linear optimization is empty";
        return false;
    }

    auto status = solution->getStatus();
    if (status == MipStatus::OPTIMAL || status == MipStatus::FEASIBLE)
    {
        return true;
    }

    logs.error() << "Problem during linear optimization";
    return false;
}

Modeler::Modeler(ILoader& loader, fs::path ouputPath, TableFormat tableFormat):
    loader_{loader},
    outputPath_{std::move(ouputPath)},
    tableFormat_(tableFormat)
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

IMipSolution* Modeler::subProbSolution()
{
    return subProbSolution_;
}

SimulationTable Modeler::makeSimulationTable(
  const IMipSolution* solution,
  const OptimEntityContainer& subproblemOptimEntityContainer,
  const FillContext& timeScenarioCtx) const
{
    // gp : subproblem_1_1 is defined the same way in multiple places
    auto& subproblem_1_1 = subproblems_[0];

    SimulationTable simulationTable;

    FillSimulationTable(simulationTable,
                        *subproblem_1_1,
                        solution->getObjectiveValue(),
                        data_,
                        subproblemOptimEntityContainer,
                        timeScenarioCtx,
                        0,
                        IO::Outputs::TimeConversionMode::SingleBlock);
    return simulationTable;
}

void Modeler::exportMps() const
{
    // 1-1.mps
    if (auto& subproblem_1_1 = subproblems_[0])
    {
        const auto mps = IO::Outputs::MPSGenerator(*subproblem_1_1, "1-1").run();
        Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / "1-1.mps", mps);
    }
    // master.mps
    if (masterProblem_)
    {
        const auto mps = IO::Outputs::MPSGenerator(*masterProblem_, "master").run();
        Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / "master.mps", mps);
    }
}

void Modeler::exportStructureFile() const
{
    // structure.txt
    const BendersDecompositionWriter writer(data_.bendersDecomposition);
    std::ofstream of(outputPath_ / "structure.txt");
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
    if (parameters_.exportMps)
    {
        exportMps();
        exportStructureFile();
    }
    if (data_.resolutionMode == ResolutionMode::SEQUENTIAL_SUBPROBLEMS)
    {
        subProbSolution_ = solveSubproblem();
        if (!checkSolution(subProbSolution_))
        {
            return;
        }

        if (!parameters_.noOutput)
        {
            auto simulationTable = makeSimulationTable(subProbSolution_,
                                                       *subproblemOptimEntityContainer_,
                                                       *timeScenarioCtx_);

            auto outputFile = outputPath_ / "simulation-table";
            ITableWriter::Ptr writer = makeTableWriter(tableFormat_, outputFile);
            writer->writeTable(simulationTable);

            logs.info() << "Simulation table is written in: " << outputFile.string();
        }
    }
}

} // namespace Antares::Solver
