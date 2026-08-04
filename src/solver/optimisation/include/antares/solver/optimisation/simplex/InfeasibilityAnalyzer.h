// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_SOLVER_OPTIMISATION_SIMPLEX_INFEASIBILITY_ANALYZER_H
#define ANTARES_SOLVER_OPTIMISATION_SIMPLEX_INFEASIBILITY_ANALYZER_H

#include <memory>
#include <string>

#include "antares/optimization-options/options.h"
#include "antares/solver/optimisation/LegacyOrtoolsLinearProblem.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/utils/opt_period_string_generator.h"
#include "antares/writer/i_writer.h"

namespace Antares::Solver::Optimization::Simplex
{

/**
 * @brief Analyzes infeasible linear problems.
 *
 * When the solver fails to find a solution, this class:
 * 1. Duplicates the LP filling logic (same as success path)
 * 2. Runs the infeasibility analyzer on the duplicated problem
 * 3. Exports an MPS file for error diagnostics
 * 4. Prints a human-readable report
 *
 * This eliminates the code duplication that existed in the error path
 * of the original OPT_AppelDuSimplexe function.
 */
class InfeasibilityAnalyzer final
{
public:
    /**
     * @brief Analyze an infeasible problem and produce diagnostics.
     *
     * @param problemeHebdo The weekly optimization problem.
     * @param originalProblem The original LP problem (for MPS export on error).
     * @param options Solver options.
     * @param NumIntervalle The interval number.
     * @param periodString Period string generator for MPS filename.
     * @param writer Result writer for MPS export.
     * @param optimizationNumber Which optimization pass (1st or 2nd).
     * @return Always returns false to signal infeasibility to the caller.
     */
    static bool analyze(PROBLEME_HEBDO* problemeHebdo,
                        const std::shared_ptr<Antares::Optimization::LegacyOrtoolsLinearProblem>& originalProblem,
                        const SingleOptimOptions& options,
                        int NumIntervalle,
                        const OptPeriodStringGenerator& periodString,
                        Solver::IResultWriter& writer,
                        int optimizationNumber);
};

} // namespace Antares::Solver::Optimization::Simplex

#endif // ANTARES_SOLVER_OPTIMISATION_SIMPLEX_INFEASIBILITY_ANALYZER_H
