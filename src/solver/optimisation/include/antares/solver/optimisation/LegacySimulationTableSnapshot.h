// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>

#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

#include "LegacyExtraOutputs.h"
#include "LegacyNameMapper.h"

struct PROBLEME_HEBDO;

namespace Antares::LinearProblem
{
class OptimEntityContainer;

namespace Api
{
class ILinearProblem;
}
} // namespace Antares::LinearProblem

namespace Antares::Optimization
{

class InactiveComponentsAnalyzer;

/**
 * \brief The modeler side of a week's last optimisation pass, kept alive past
 * the solve so a post-process dump can re-emit the modeler component rows.
 *
 * Cheap to hold: the MPSolver owning the problem data stays in
 * PROBLEME_ANTARES_A_RESOUDRE::ProblemesSpx meanwhile, so this only adds the
 * wrapper objects. OPT_TryToCallSimplex drops the previous value before the next
 * problem, so two are never alive at once.
 */
struct SolvedModelerProblem
{
    std::shared_ptr<const LinearProblem::Api::ILinearProblem> problem;
    std::shared_ptr<const LinearProblem::OptimEntityContainer> entities;
    double objectiveValue = 0.;
};

/**
 * \brief Write the legacy solver's contribution to the simulation table.
 *
 * One raw row per named optimisation variable (value `solution.primal[i]`, name
 * from nameMapper), then the derived rows of AddLegacyExtraOutputs. The normal
 * solve passes a view of PROBLEME_ANTARES_A_RESOUDRE's X / duals; a post-process
 * dump passes the post-processed values.
 */
void FillLegacySimulationTable(IO::Outputs::SimulationTable& simulationTable,
                               PROBLEME_HEBDO& problemeHebdo,
                               const LegacySolution& solution,
                               const LinearProblem::Api::FillContext& fillContext,
                               const LegacyNameMapper& nameMapper,
                               unsigned currentBlock,
                               const InactiveComponentsAnalyzer* inactiveComponents = nullptr);

/// \brief Block index of the week problemeHebdo currently holds. Same expression
/// as the weekly branch of OPT_TryToCallSimplex, for callers that run once per
/// week rather than once per optimisation interval.
unsigned LegacyWeeklyBlock(const PROBLEME_HEBDO& problemeHebdo);

/**
 * \brief Publish the results as they stand *after* post-processing.
 *
 * Reconstructs the post-processed primal and dual values by reading them back
 * through the solver's address tables (post-processes mutate the legacy result
 * structures in place, never ProblemeAResoudre::X), then fills from those
 * locals -- the solver state is never touched, so this is observation-only. In
 * hybrid mode the modeler component rows are re-emitted first, from
 * PROBLEME_HEBDO::lastSolvedModelerProblem; those keep the values the solver
 * left, as post-processing cannot move a modeler variable.
 *
 * No-op with a one-time warning when the simplex range is daily: the address
 * table is then rebuilt per interval, so only the last day would be readable.
 *
 * See docs/architecture/legacy-extra-outputs.md §8.
 */
void DumpSimulationTableAfterPostProcess(
  IO::Outputs::SimulationTable& simulationTable,
  PROBLEME_HEBDO& problemeHebdo,
  const LinearProblem::Api::FillContext& fillContext,
  unsigned currentBlock,
  const InactiveComponentsAnalyzer* inactiveComponents = nullptr);

/**
 * \brief Dump into the `stage` table of a year's tables, deriving the week's
 * fill context and block from problemeHebdo.
 *
 * No-op when `tables` is null (run writes no simulation tables). In its own
 * translation unit because it needs buildFillContext, which drags in the solver
 * call chain; callers that already have a fill context use the overload above.
 */
void DumpSimulationTableStage(IO::Outputs::OptimisationsSimulationTable* tables,
                              IO::Outputs::Stage stage,
                              PROBLEME_HEBDO& problemeHebdo);

} // namespace Antares::Optimization
