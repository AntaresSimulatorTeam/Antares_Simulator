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
#include "antares/writer/simulation_table_writer.h"

using namespace Antares;
using namespace Antares::Writer;
using namespace Antares::LinearProblem::MpsolverImpl;
using namespace Antares::Optimization;
using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::Api;
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

fs::path makeOutputPath(fs::path studyPath)
{
    const auto simulationId = formatTime(getCurrentTime(), "%Y%m%d-%H%M");
    fs::path outputPath = studyPath / "output" / simulationId;

    // avoid overwriting existing output by adding a suffix (-2, -3, etc.)
    if (!Utils::generatePathWithSuffix(outputPath))
    {
        throw Modeler::ModelerError("Output folder already exists: " + outputPath.string());
    }

    logs.info() << "Output folder : " << outputPath;
    if (!fs::is_directory(outputPath) && !fs::create_directories(outputPath))
    {
        throw Modeler::ModelerError("Failed to create output directory. Exiting simulation.");
    }
    return outputPath;
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

namespace
{

// Creates a filler for each system component and fills the linear problem.
void buildSystemProblem(const ModelerStudy::SystemModel::System& system,
                        const ILinearProblemData& data,
                        const ScenarioGroupRepository& scenarioGroupRepository,
                        BendersDecomposition& bendersDecomposition,
                        OptimEntityContainer& optimEntityContainer,
                        const FillContext& timeScenarioCtx,
                        Config::Location location)
{
    std::vector<std::unique_ptr<LinearProblemFiller>> fillers;
    const auto& components = system.Components();
    optimEntityContainer.addFromSystemComponents(components, location);

    for (const auto& component: components)
    {
        auto filler = std::make_unique<ComponentFiller>(component,
                                                        &data,
                                                        optimEntityContainer,
                                                        scenarioGroupRepository,
                                                        location,
                                                        &bendersDecomposition);
        fillers.push_back(std::move(filler));
    }

    LinearProblemBuilder linear_problem_builder(fillers);
    linear_problem_builder.build(timeScenarioCtx);
}

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

std::shared_ptr<ILinearProblem> getProblem(bool isMip,
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
        return std::make_shared<OrtoolsLinearProblem>(isMip, solver.value());
    }
    return std::make_shared<StructuredLinearProblem>();
}

} // namespace

std::unique_ptr<OptimEntityContainer> buildProblem(ModelerData& data,
                                                   const Config::Location& location,
                                                   const std::string& problemId,
                                                   const FillContext& timeScenarioCtx,
                                                   const ResolutionMode& resolutionMode,
                                                   const std::optional<std::string>& solver)
{
    auto [hasCompatibleVariable, isMip] = analyzeLocation(data, location);
    if (!hasCompatibleVariable)
    {
        return nullptr;
    }
    // The container shares ownership of the problem, keeping it alive for
    // post-solve consumers that read variable solution values through it.
    auto problem = getProblem(isMip, resolutionMode, solver);
    auto container = std::make_unique<OptimEntityContainer>(problem);
    data.bendersDecomposition.setCurrentProblemId(problemId);
    buildSystemProblem(*data.system,
                       *data.dataSeries,
                       data.scenarioGroupRepository,
                       data.bendersDecomposition,
                       *container,
                       timeScenarioCtx,
                       location);
    return container;
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
        const auto mps = IO::Outputs::MPSGenerator(*subproblem_1_1, "1-1", true).run();
        Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / "1-1.mps", mps);
    }
    // master.mps
    if (masterProblem_)
    {
        const auto mps = IO::Outputs::MPSGenerator(*masterProblem_, "master", true).run();
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

    if (auto container = buildProblem(data_,
                                      Config::Location::MASTER,
                                      "master",
                                      *timeScenarioCtx_,
                                      ResolutionMode::BENDERS_DECOMPOSITION,
                                      std::nullopt))
    {
        masterProblem_ = container->Problem();
    }

    subproblemOptimEntityContainer_ = buildProblem(data_,
                                                   Config::Location::SUBPROBLEMS,
                                                   "1-1",
                                                   *timeScenarioCtx_,
                                                   data_.resolutionMode,
                                                   parameters_.solver);
    if (subproblemOptimEntityContainer_)
    {
        subproblems_.emplace_back(subproblemOptimEntityContainer_->Problem());
    }

    logs.info() << "Linear problem provided";

    auto& subproblem_1_1 = subproblems_[0];
    logs.info() << "Number of variables: " << subproblem_1_1->variableCount();
    logs.info() << "Number of constraints: " << subproblem_1_1->constraintCount();

    measure.tick();
    logs.info();
    logs.info() << "Modeler build took " << measure.toStringInSeconds();
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
            SimulationTableWriter writer(outputFile, tableFormat_);
            writer.writeTable(simulationTable);
            logs.info() << "Simulation table is written in: " << outputFile.string();
        }
    }
}

} // namespace Antares::Solver
