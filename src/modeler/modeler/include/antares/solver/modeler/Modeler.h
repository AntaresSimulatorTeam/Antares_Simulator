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

namespace Antares::Optimisation
{
class BendersDecomposition;

namespace LinearProblemApi
{
/** \brief Context for filling linear problem data.
 * Contains temporal information
 */
class FillContext;
class IMipSolution;
} // namespace LinearProblemApi
} // namespace Antares::Optimisation

namespace Antares::Solver
{
class ILoader;

struct ProblemEntity
{
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> problem;
    std::unique_ptr<Optimisation::OptimEntityContainer> optimEntityContainer;
};

ProblemEntity buildProblem(const Antares::Solver::ModelerData& data,
                           const Config::Location& location,
                           const std::string& problemId,
                           Optimisation::BendersDecomposition* bendersDecomposition,
                           const Optimisation::LinearProblemApi::FillContext& timeScenarioCtx,
                           const ResolutionMode& resolutionMode,
                           const std::optional<std::string>& solver);

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

    [[nodiscard]] const std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>&
    masterProblem() const
    {
        return masterProblem_;
    }

    [[nodiscard]] const std::vector<
      std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>>&
    subproblems() const
    {
        return subproblems_;
    }

    // gp : defined only for unit test, which is not a good sign on design.
    Optimisation::LinearProblemApi::IMipSolution* subProbSolution();

private:
    Optimisation::LinearProblemApi::IMipSolution* solveSubproblem();

    IO::Outputs::SimulationTable makeSimulationTable(
      const Optimisation::LinearProblemApi::IMipSolution* solution,
      const Optimisation::OptimEntityContainer& subproblemOptimEntityContainer,
      const Optimisation::LinearProblemApi::FillContext& timeScenarioCtx) const;

    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> masterProblem_ = nullptr;
    std::vector<std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>> subproblems_;
    std::unique_ptr<Optimisation::OptimEntityContainer> subproblemOptimEntityContainer_ = nullptr;
    std::unique_ptr<Optimisation::LinearProblemApi::FillContext> timeScenarioCtx_ = nullptr;
    Optimisation::LinearProblemApi::IMipSolution* subProbSolution_ = nullptr;
    ModelerParameters parameters_;
    ModelerData data_;
    fs::path outputPath_;
    Antares::Writer::TableFormat tableFormat_;
};
} // namespace Antares::Solver
