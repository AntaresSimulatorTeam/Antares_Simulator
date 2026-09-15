// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/modeler/Modeler.h"

#include <algorithm>
#include <fmt/format.h>
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

    scenarios_ = resolveScenarioScopeScenarios(data_.scenarioScope);
    validateScenariosAgainstScenarioBuilder();
    logs.info() << fmt::format("Number of Monte-Carlo scenarios to simulate: {}",
                               scenarios_.size());
}

void Modeler::validateScenariosAgainstScenarioBuilder() const
{
    // Validate per component: a component with an empty scenario group id uses the default
    // scenario (always valid), and an unknown group is reported by
    // ScenarioGroupRepository::scenario() when the problems are built. Each component is
    // only required to have its own scenario group cover the selected years; other groups
    // used by different components are independent.
    std::vector<std::string> invalidEntries;
    for (const auto& component: data_.system->Components())
    {
        const auto& groupId = component.getScenarioGroupId();
        if (groupId.empty() || !data_.scenarioGroupRepository.contains(groupId))
        {
            continue;
        }
        const auto& scenario = data_.scenarioGroupRepository.scenario(groupId);
        for (const auto year: scenarios_)
        {
            if (!scenario.hasYear(year))
            {
                invalidEntries.push_back(
                  fmt::format("scenario {} (no time series in scenario group '{}' used by "
                              "component '{}')",
                              year,
                              scenario.group(),
                              component.Id()));
            }
        }
    }
    if (!invalidEntries.empty())
    {
        std::string joined = invalidEntries.front();
        for (std::size_t i = 1; i < invalidEntries.size(); ++i)
        {
            joined += ", " + invalidEntries[i];
        }
        throw ModelerError(fmt::format(
          "scenario-scope selection is not valid: the following scenario indices are not defined "
          "in the scenario builder (modeler-scenariobuilder.dat): {}",
          joined));
    }
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

std::string makeProblemId(unsigned year)
{
    return std::to_string(year) + "-0";
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

IMipSolution* Modeler::solveSubproblem(ILinearProblem& subproblem)
{
    Utils::TimeMeasurement measure;
    logs.info() << "Launching resolution...";
    measure.reset();
    auto* solution = subproblem.solve(parameters_.solverLogs);
    measure.tick();
    logs.info() << "Solved in " << measure.toStringInSeconds();
    return solution;
}

IMipSolution* Modeler::subProbSolution()
{
    return subProbSolution_;
}

SimulationTable& Modeler::fillSimulationTable(
  SimulationTable& simulationTable,
  const IMipSolution* solution,
  const ILinearProblem& subproblem,
  const OptimEntityContainer& subproblemOptimEntityContainer,
  const FillContext& timeScenarioCtx) const
{
    FillSimulationTable(simulationTable,
                        subproblem,
                        solution->getObjectiveValue(),
                        data_,
                        subproblemOptimEntityContainer,
                        timeScenarioCtx,
                        0,
                        IO::Outputs::TimeConversionMode::SingleBlock);
    return simulationTable;
}

void Modeler::buildProblemsAndWriteMps()
{
    buildProblems();
    for (std::size_t i = 0; i < subproblems_.size(); ++i)
    {
        auto& subproblem = subproblems_[i];
        if (!subproblem)
        {
            continue;
        }
        const auto name = makeProblemId(scenarios_[i]);
        const auto mps = IO::Outputs::MPSGenerator(*subproblem, name, true).run();
        Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / (name + ".mps"), mps);
    }
    if (masterProblem_)
    {
        const auto mps = IO::Outputs::MPSGenerator(*masterProblem_, "master", true).run();
        Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / "master.mps", mps);

        logs.info() << "Master number of variables: " << masterProblem_->variableCount();
        logs.info() << "Master number of constraints: " << masterProblem_->constraintCount();
    }

    exportStructureFile();
}

void Modeler::exportStructureFile() const
{
    // structure.txt
    const BendersDecompositionWriter writer(data_.bendersDecomposition);
    std::ofstream of(outputPath_ / "structure.txt");
    writer.write(of);
}

ProblemEntity Modeler::buildSubProblem(unsigned year)
{
    return buildProblem(data_,
                        Config::Location::SUBPROBLEMS,
                        makeProblemId(year),
                        &data_.bendersDecomposition,
                        createFillContext(year),
                        data_.resolutionMode,
                        parameters_.solver);
}

void Modeler::buildMasterProblem()
{
    auto masterFillContext = createFillContext(0); // master is scenario-independent
    auto masterEntities = buildProblem(data_,
                                       Config::Location::MASTER,
                                       "master",
                                       &data_.bendersDecomposition,
                                       masterFillContext,
                                       ResolutionMode::BENDERS_DECOMPOSITION,
                                       std::nullopt);
    masterProblem_ = std::move(masterEntities.problem);
}

FillContext Modeler::createFillContext(unsigned year) const
{
    return FillContext(
      parameters_.firstTimeStep,
      parameters_.lastTimeStep,
      parameters_.firstTimeStep, // global = local, single time block in pure modeler (for now)
      parameters_.lastTimeStep,  // global = local
      year);
}

void Modeler::buildProblems()
{
    subproblems_.clear();
    subproblemOptimEntityContainers_.clear();
    Utils::TimeMeasurement measure;

    logs.info() << "linear problem of System loaded";

    buildMasterProblem();

    for (const unsigned year: scenarios_)
    {
        auto entities = buildSubProblem(year);
        subproblems_.emplace_back(std::move(entities.problem));
        subproblemOptimEntityContainers_.emplace_back(std::move(entities.optimEntityContainer));
    }

    measure.tick();
    logs.info();
    logs.info() << "Modeler build took " << measure.toStringInSeconds();

    // A scenario whose components expose no variable at the subproblem location yields a
    // null problem (see buildProblem). Count only the non-null problems and read the
    // statistics from the first one, never from an entry that may be null.
    const auto firstNonNull = std::find_if(subproblems_.begin(),
                                           subproblems_.end(),
                                           [](const auto& problem) { return problem != nullptr; });
    const std::size_t nonNullCount = std::count_if(subproblems_.begin(),
                                                   subproblems_.end(),
                                                   [](const auto& problem)
                                                   { return problem != nullptr; });

    if (nonNullCount == 0)
    {
        logs.warning()
          << "No subproblem was built. Check your scenario-scope and modeler parameters.";
    }
    else
    {
        logs.info() << "Number of subproblems built: " << nonNullCount;
        logs.info() << "Number of variables: " << firstNonNull->get()->variableCount();
        logs.info() << "Number of constraints: " << firstNonNull->get()->constraintCount();
    }
}

void Modeler::run()
{
    if (data_.resolutionMode == ResolutionMode::BENDERS_DECOMPOSITION)
    {
        if (!parameters_.exportMps)
        {
            logs.error() << "Resolution mode is benders-decomposition but exportMps is false. No "
                            "resolution will be performed and no problem will be exported.";
            throw ModelerError("Conflicting parameters: benders-decomposition and exportMps");
        }

        buildProblemsAndWriteMps();
    }
    else if (data_.resolutionMode == ResolutionMode::SEQUENTIAL_SUBPROBLEMS)
    {
        buildMasterProblem();

        bool masterMpsWritten = false;
        for (const unsigned year: scenarios_)
        {
            auto fillContext = createFillContext(year);
            auto entities = buildSubProblem(year);

            if (!entities.problem)
            {
                logs.warning() << fmt::format("No subproblem was built for scenario {}: skipping",
                                              year);
                continue;
            }

            logs.info() << "Number of variables: " << entities.problem->variableCount();
            logs.info() << "Number of constraints: " << entities.problem->constraintCount();

            if (parameters_.exportMps)
            {
                const auto name = makeProblemId(year);
                const auto mps = IO::Outputs::MPSGenerator(*entities.problem, name, true).run();
                Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / (name + ".mps"), mps);

                if (!masterMpsWritten && masterProblem_)
                {
                    const auto masterMps = IO::Outputs::MPSGenerator(*masterProblem_,
                                                                     "master",
                                                                     true)
                                             .run();
                    Antares::IO::Outputs::MPSFileWriter::write(outputPath_ / "master.mps",
                                                               masterMps);
                    masterMpsWritten = true;
                }
            }

            logs.info() << "Solving scenario " << year;
            subProbSolution_ = solveSubproblem(*entities.problem);

            // solve() returns a solution owned by the subproblem (see ILinearProblem::solve),
            // so retain the problem right away: subProbSolution_ is only valid while its
            // owning subproblem is alive. Keeping only the most recent subproblem in memory
            // also guarantees the invariant on every exit path, including the early return
            // below: subProbSolution_ is either null or points into a problem held in
            // subproblems_.
            subproblems_.clear();
            subproblemOptimEntityContainers_.clear();
            subproblems_.emplace_back(std::move(entities.problem));
            subproblemOptimEntityContainers_.emplace_back(std::move(entities.optimEntityContainer));

            if (!checkSolution(subProbSolution_))
            {
                return;
            }

            auto& problem = *subproblems_.back();
            auto& optimEntityContainer = *subproblemOptimEntityContainers_.back();

            if (!parameters_.noOutput)
            {
                SimulationTable simulationTable;
                fillSimulationTable(simulationTable,
                                    subProbSolution_,
                                    problem,
                                    optimEntityContainer,
                                    fillContext);

                auto outputFile = outputPath_ / ("simulation-table-" + std::to_string(year));
                SimulationTableWriter writer(outputFile, tableFormat_);
                writer.writeTable(simulationTable);
                logs.info() << "Simulation table of scenario " << year
                            << " is written in: " << writer.outputFile().string();
            }
        }
    }
}

} // namespace Antares::Solver
