// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>

#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

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

// The modeler side of the last optimisation pass of a week, kept alive past the
// solve so a post-process dump can re-emit the modeler component rows.
//
// Cheap to hold, but only because the lifetime is kept tight at both ends. The
// MPSolver holding the actual problem data is also in
// PROBLEME_ANTARES_A_RESOUDRE::ProblemesSpx for as long as this lives, so what
// the retention adds is the wrapper objects -- and OrtoolsMipVariable only wraps
// a raw MPVariable*. OPT_TryToCallSimplex drops the previous value before
// building the next problem, so two are never alive at once, and neither
// survives the pass that replaced it.
struct SolvedModelerProblem
{
    std::shared_ptr<const Antares::LinearProblem::Api::ILinearProblem> problem;
    std::shared_ptr<const Antares::LinearProblem::OptimEntityContainer> entities;
    double objectiveValue = 0.;
};

// Writes the legacy solver's contribution to the simulation table: one raw row
// per named optimisation variable (value = X[i], name translated through
// nameMapper), followed by the derived rows of AddLegacyExtraOutputs.
//
// Reads the solution through PROBLEME_ANTARES_A_RESOUDRE::X, so the caller is
// responsible for X holding the state it wants published.
void FillLegacySimulationTable(Antares::IO::Outputs::SimulationTable& simulationTable,
                               PROBLEME_HEBDO& problemeHebdo,
                               const Antares::LinearProblem::Api::FillContext& fillContext,
                               const LegacyNameMapper& nameMapper,
                               unsigned currentBlock);

// Block index of the week problemeHebdo currently holds. Same expression as the
// weekly branch of OPT_TryToCallSimplex, for callers that run once per week
// rather than once per optimisation interval.
unsigned LegacyWeeklyBlock(const PROBLEME_HEBDO& problemeHebdo);

// Publishes the results as they stand *after* post-processing.
//
// In hybrid mode the modeler component rows are re-emitted first, from
// PROBLEME_HEBDO::lastSolvedModelerProblem, so a stage table describes the whole
// system rather than only its legacy half. Their values are necessarily the ones
// the solver left: post-processing mutates the legacy result structures only, and
// cannot move a modeler variable.
//
// Post-processes (remix hydro / shave-peaks, the adequacy patch, ...) mutate
// PROBLEME_HEBDO's result structures in place and never write back into
// ProblemeAResoudre::X, so a plain fill would re-emit the values the solver
// left. AdresseOuPlacerLaValeurDesVariablesOptimisees already points at the
// exact result slot of each variable -- OPT_AppelDuSimplexe publishes with
// `*address = X[i]` -- so reading those addresses back into X republishes the
// post-processed state and lets the regular fill be reused unchanged. The same
// holds for the duals through AdresseOuPlacerLaValeurDesCoutsMarginaux, which
// UpdateMrgPriceAfterCSRcmd overwrites in place.
//
// X and the duals are restored before returning, so calling this cannot change
// anything the simulation computes afterwards.
//
// Does nothing (with a one-time warning) when the simplex optimization range is
// daily: the week is then solved in seven intervals that each rebuild the
// address table, so only the last day would be readable here.
void DumpSimulationTableAfterPostProcess(
  Antares::IO::Outputs::SimulationTable& simulationTable,
  PROBLEME_HEBDO& problemeHebdo,
  const Antares::LinearProblem::Api::FillContext& fillContext,
  unsigned currentBlock);

// Dumps into the `stage` table of a year's tables, deriving the week's fill
// context and block from problemeHebdo. No-op when `tables` is null, i.e. when
// the run does not write simulation tables.
//
// Defined in its own translation unit because it needs buildFillContext, which
// drags in the solver call chain; callers that already have a fill context
// should use the overload above.
void DumpSimulationTableStage(Antares::IO::Outputs::OptimisationsSimulationTable* tables,
                              const std::string& stage,
                              PROBLEME_HEBDO& problemeHebdo);

} // namespace Antares::Optimization
