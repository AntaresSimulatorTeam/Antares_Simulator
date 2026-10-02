// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_SOLVER_OPTIMISATION_SIMPLEX_SIMPLEX_RESULT_H
#define ANTARES_SOLVER_OPTIMISATION_SIMPLEX_SIMPLEX_RESULT_H

#include <memory>

#include "antares/solver/optimisation/LegacyOrtoolsLinearProblem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Optimization::Simplex
{

/**
 * @brief Holds the result of a simplex solver invocation.
 *
 * Contains timing measurements, the original LP problem reference,
 * the objective value, and whether the solver found a valid solution.
 */
struct SimplexResult final
{
    TIME_MEASURE timeMeasure;
    std::shared_ptr<Antares::Optimization::LegacyOrtoolsLinearProblem> originalProblem;
    double objectiveValue = 0.0;
    bool success = false;
};

/**
 * @brief Apply solver results back into PROBLEME_HEBDO.
 *
 * Copies variable values (X), reduced costs (CoutsReduits), and marginal costs
 * (CoutsMarginaux) from the solved problem back to their destination addresses.
 * Also stores timing and optimization cost values.
 *
 * @param problemeHebdo The weekly optimization problem to update.
 * @param result The simplex solver result containing X, duals, costs.
 * @param NumIntervalle The interval number (used for cost array indexing).
 * @param optimizationNumber Which optimization pass (1st or 2nd).
 */
void applyResults(PROBLEME_HEBDO& problemeHebdo,
                  const SimplexResult& result,
                  int NumIntervalle,
                  int optimizationNumber);

} // namespace Antares::Solver::Optimization::Simplex

#endif // ANTARES_SOLVER_OPTIMISATION_SIMPLEX_SIMPLEX_RESULT_H
