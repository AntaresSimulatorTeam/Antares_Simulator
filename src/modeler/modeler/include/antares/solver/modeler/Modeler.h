// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <memory>

#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include "antares/io/outputs/SimulationTable.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/modeler/parameters/modelerParameters.h"
#include "antares/solver/optim-model-filler/BendersDecomposition.h"
#include "antares/writer/table_format.h"

#include "ModelerData.h"

namespace fs = std::filesystem;

namespace Antares::LinearProblem
{
class BendersDecomposition;

namespace Api
{
/** \brief Context for filling linear problem data.
 * Contains temporal information
 */
class FillContext;
class IMipSolution;
} // namespace Api
} // namespace Antares::LinearProblem

namespace Antares::Solver
{
class ILoader;

// Returns the shared problem, or nullptr if no variable is compatible with the location.
std::shared_ptr<LinearProblem::Api::ILinearProblem> buildProblem(
  const Antares::Solver::ModelerData& data,
  const Config::Location& location,
  const std::string& problemId,
  LinearProblem::BendersDecomposition* bendersDecomposition,
  const LinearProblem::Api::FillContext& timeScenarioCtx,
  const ResolutionMode& resolutionMode,
  const std::optional<std::string>& solver);

// Returns the optimisation entity container (which shares the problem's lifetime),
// or nullptr if buildProblem did not build a problem for the location.
std::unique_ptr<LinearProblem::OptimEntityContainer> buildSubProblemContainer(
  Antares::Solver::ModelerData& data,
  const LinearProblem::Api::FillContext& timeScenarioCtx,
  const std::optional<std::string>& solver);

std::filesystem::path makeOutputPath(std::filesystem::path studyPath);

class Modeler final
{
public:
    Modeler(ILoader& loader, fs::path outputPath, Antares::Writer::TableFormat tableFormat);

    void buildProblems();
    void buildMasterProblem();
    void buildProblemsAndWriteMps();
    void run();

    void exportStructureFile() const;

    class ModelerError: public std::runtime_error
    {
    public:
        explicit ModelerError(const std::string& s):
            runtime_error(s)
        {
        }
    };

    ILoader& loader_; // gp : make it private

    [[nodiscard]] const std::shared_ptr<LinearProblem::Api::ILinearProblem>& masterProblem() const
    {
        return masterProblem_;
    }

    [[nodiscard]]
    const std::vector<std::shared_ptr<LinearProblem::Api::ILinearProblem>>& subproblems() const
    {
        return subproblems_;
    }

    // gp : defined only for unit test, which is not a good sign on design.
    /// Returns the solution of the last solved subproblem. The solution is owned by that
    /// subproblem (see ILinearProblem::solve), which is retained in subproblems(), so the
    /// pointer stays valid for the lifetime of this Modeler after run() in
    /// SEQUENTIAL_SUBPROBLEMS mode. May be null if no subproblem was solved.
    LinearProblem::Api::IMipSolution* subProbSolution();

private:
    LinearProblem::Api::FillContext createFillContext(unsigned year) const;
    ProblemEntity buildSubProblem(unsigned year);
    void validateScenariosAgainstScenarioBuilder() const;
    LinearProblem::Api::IMipSolution* solveSubproblem(
      LinearProblem::Api::ILinearProblem& subproblem);

    IO::Outputs::SimulationTable& fillSimulationTable(
      IO::Outputs::SimulationTable& simulationTable,
      const LinearProblem::Api::IMipSolution* solution,
      const LinearProblem::Api::ILinearProblem& subproblem,
      const LinearProblem::OptimEntityContainer& subproblemOptimEntityContainer,
      const LinearProblem::Api::FillContext& timeScenarioCtx) const;

    std::unique_ptr<LinearProblem::Api::ILinearProblem> masterProblem_ = nullptr;
    std::vector<std::unique_ptr<LinearProblem::Api::ILinearProblem>> subproblems_;
    std::vector<std::unique_ptr<LinearProblem::OptimEntityContainer>>
      subproblemOptimEntityContainers_;
    std::vector<unsigned> scenarios_;

    ModelerParameters parameters_;
    ModelerData data_;
    fs::path outputPath_;
    Antares::Writer::TableFormat tableFormat_;
    LinearProblem::Api::IMipSolution* subProbSolution_ = nullptr;
};
} // namespace Antares::Solver
