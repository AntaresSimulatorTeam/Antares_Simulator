// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/optimisation/linear-problem-api/linearProblem.h>
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/modeler/parameters/modelerParameters.h"
#include "antares/solver/optim-model-filler/BendersDecomposition.h"

#include "ModelerData.h"

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
class IWriter;

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
    Modeler(ILoader& loader, IWriter& writer);

    void buildProblems();
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

    ILoader& loader_;
    IWriter& writer_;

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

private:
    Optimisation::LinearProblemApi::IMipSolution* solveSubproblem();

    void buildMasterProblem();
    void buildSubProblem();

    void writeSubProblemSimulationTable(
      const Optimisation::LinearProblemApi::IMipSolution* solution,
      const Optimisation::LinearProblemApi::FillContext& timeScenarioCtx) const;
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem> masterProblem_ = nullptr;
    std::vector<std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblem>> subproblems_;
    std::unique_ptr<Optimisation::OptimEntityContainer> subproblemOptimEntityContainer_ = nullptr;
    std::unique_ptr<Optimisation::LinearProblemApi::FillContext> timeScenarioCtx_ = nullptr;
    ModelerParameters parameters_;
    ModelerData data_;
};
} // namespace Antares::Solver
