// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_SOLVER_OPTIMISATION_SIMPLEX_SIMPLEX_ORCHESTRATOR_H
#define ANTARES_SOLVER_OPTIMISATION_SIMPLEX_SIMPLEX_ORCHESTRATOR_H

#include <memory>
#include <optional>
#include <ortools/linear_solver/linear_solver.h>
#include <string>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/optimization-options/options.h"
#include "antares/solver/optimisation/LegacyOrtoolsLinearProblem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/simplex/SimplexResult.h"
#include "antares/solver/utils/opt_period_string_generator.h"
#include "antares/writer/i_writer.h"

namespace Antares::Solver::Optimization::Simplex
{

/**
 * @brief Orchestrates the full simplex solve flow for a single subproblem.
 *
 * Encapsulates the 7-step solve flow:
 * 1. Create LP problem
 * 2. Fill LP with variables, constraints, objectives
 * 3. Export MPS if configured
 * 4. Call the OR-Tools solver
 * 5. Handle success/failure results
 * 6. Fill simulation tables
 * 7. Return structured result
 *
 * This replaces the monolithic OPT_TryToCallSimplex function.
 */
class SimplexOrchestrator final
{
public:
    SimplexOrchestrator(SingleOptimOptions options,
                        PROBLEME_HEBDO& problemeHebdo,
                        int NumIntervalle,
                        int optimizationNumber,
                        const OptPeriodStringGenerator& periodString,
                        Solver::IResultWriter& writer,
                        IO::Outputs::SimulationTable* simulationTable);

    /**
     * @brief Execute the full solve flow.
     * @return SimplexResult with timing, objective value, and success flag.
     */
    SimplexResult solve();

private:
    void exportMps();
    void fillSimulationTable();

    /**
     * @brief Handle solver outcome: reset on failure, throw on internal error.
     *
     * @return true if the solver found a valid solution (OUI_SPX),
     *         false if the solver failed (caller should return infeasible result).
     * @throws FatalError if the solver reported an internal error (SPX_ERREUR_INTERNE).
     */
    bool handleSolve() const;

    /**
     * @brief Create and fill the LP problem.
     */
    void createAndFillLp();

    // Configuration
    SingleOptimOptions options_;
    PROBLEME_HEBDO& problemeHebdo_;
    int NumIntervalle_;
    int optimizationNumber_;
    const OptPeriodStringGenerator& periodString_;
    Solver::IResultWriter& writer_;
    IO::Outputs::SimulationTable* simulationTable_;

    // Problem state
    std::shared_ptr<Antares::Optimization::LegacyOrtoolsLinearProblem> ortoolsProblem_;
    std::optional<Optimisation::LinearProblemApi::FillContext> fillCtx_;
    std::unique_ptr<Optimisation::OptimEntityContainer> optimEntityContainer_;
    std::shared_ptr<operations_research::MPSolver> solver_;

    // Timing
    Utils::TimeMeasurement measure_;
    TIME_MEASURE timeMeasure_;
};

} // namespace Antares::Solver::Optimization::Simplex

#endif // ANTARES_SOLVER_OPTIMISATION_SIMPLEX_SIMPLEX_ORCHESTRATOR_H
