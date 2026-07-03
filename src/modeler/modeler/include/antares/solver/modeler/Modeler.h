// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

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

struct ProblemEntity
{
    std::unique_ptr<LinearProblem::Api::ILinearProblem> problem;
    std::unique_ptr<LinearProblem::OptimEntityContainer> optimEntityContainer;
};

ProblemEntity buildProblem(const Antares::Solver::ModelerData& data,
                           const Config::Location& location,
                           const std::string& problemId,
                           LinearProblem::BendersDecomposition* bendersDecomposition,
                           const LinearProblem::Api::FillContext& timeScenarioCtx,
                           const ResolutionMode& resolutionMode,
                           const std::optional<std::string>& solver);

std::filesystem::path makeOutputPath(std::filesystem::path studyPath);

class Modeler final
{
public:
    Modeler(ILoader& loader, fs::path outputPath, Antares::Writer::TableFormat tableFormat);

    void buildProblems();
    void buildMasterProblem();
    void buildSubProblem();
    void run();

    void exportMps() const;
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

    [[nodiscard]] const std::unique_ptr<LinearProblem::Api::ILinearProblem>&
    masterProblem() const
    {
        return masterProblem_;
    }

    [[nodiscard]] const std::vector<
      std::unique_ptr<LinearProblem::Api::ILinearProblem>>&
    subproblems() const
    {
        return subproblems_;
    }

    // gp : defined only for unit test, which is not a good sign on design.
    LinearProblem::Api::IMipSolution* subProbSolution();

private:
    LinearProblem::Api::IMipSolution* solveSubproblem();

    IO::Outputs::SimulationTable makeSimulationTable(
      const LinearProblem::Api::IMipSolution* solution,
      const LinearProblem::OptimEntityContainer& subproblemOptimEntityContainer,
      const LinearProblem::Api::FillContext& timeScenarioCtx) const;

    std::unique_ptr<LinearProblem::Api::ILinearProblem> masterProblem_ = nullptr;
    std::vector<std::unique_ptr<LinearProblem::Api::ILinearProblem>> subproblems_;
    std::unique_ptr<LinearProblem::OptimEntityContainer> subproblemOptimEntityContainer_ = nullptr;
    std::unique_ptr<LinearProblem::Api::FillContext> timeScenarioCtx_ = nullptr;
    LinearProblem::Api::IMipSolution* subProbSolution_ = nullptr;
    ModelerParameters parameters_;
    ModelerData data_;
    fs::path outputPath_;
    Antares::Writer::TableFormat tableFormat_;
};
} // namespace Antares::Solver
