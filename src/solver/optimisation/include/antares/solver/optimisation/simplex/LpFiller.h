// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_SOLVER_OPTIMISATION_SIMPLEX_LP_FILLER_H
#define ANTARES_SOLVER_OPTIMISATION_SIMPLEX_LP_FILLER_H

#include <memory>
#include <vector>

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/solver/optim-model-filler/BendersDecomposition.h"
#include "antares/solver/optimisation/LegacyOrtoolsLinearProblem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Optimization::Simplex
{

/**
 * @brief Fills a linear problem with variables, constraints, and objectives.
 *
 * Encapsulates the logic for:
 * - Building the fill context from PROBLEME_HEBDO
 * - Creating and populating an OptimEntityContainer
 * - Invoking fillers (LegacyFiller, ComponentFiller, etc.) via LinearProblemBuilder
 */
class LpFiller final
{
public:
    /**
     * @brief Build the fill context from a weekly problem.
     * @param problemeHebdo The weekly optimization problem.
     * @param NumIntervalle The interval number within the week.
     * @return FillContext with local/global time step ranges.
     */
    static Optimisation::LinearProblemApi::FillContext buildFillContext(
      PROBLEME_HEBDO& problemeHebdo,
      int NumIntervalle);

    /**
     * @brief Fill the linear problem using the given context and entity container.
     *
     * Adds fillers based on whether modeler data is present:
     * - Always: LegacyFiller
     * - With modeler: ComponentFiller(s), ComponentToAreaConnectionFiller, ThermalCapacityFiller
     *
     * @param fillCtx Context with time step ranges.
     * @param problemeHebdo The weekly optimization problem.
     * @param optimEntityContainer Container holding the LP problem and entities.
     * @param bendersDecomposition Optional Benders decomposition data.
     */
    static void fillLinearProblem(
      const Optimisation::LinearProblemApi::FillContext& fillCtx,
      PROBLEME_HEBDO& problemeHebdo,
      Optimisation::OptimEntityContainer& optimEntityContainer,
      Optimisation::BendersDecomposition* bendersDecomposition = nullptr);

private:
    /**
     * @brief Add modeler components (system components + compatibility fillers).
     */
    static void fillModelerComponents(
      std::vector<std::unique_ptr<Optimisation::LinearProblemApi::LinearProblemFiller>>&
        fillersCollection,
      const ModelerData& modelerData,
      Optimisation::OptimEntityContainer& optimEntityContainer,
      Optimisation::BendersDecomposition* bendersDecomposition);
};

} // namespace Antares::Solver::Optimization::Simplex

#endif // ANTARES_SOLVER_OPTIMISATION_SIMPLEX_LP_FILLER_H
