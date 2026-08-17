// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

#include "LegacyNameMapper.h"

struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

// Writes the legacy solver's contribution to the simulation table: one raw row
// per named optimisation variable (value = X[i], name translated through
// nameMapper), followed by the derived rows of AddLegacyExtraOutputs.
//
// Reads the solution through PROBLEME_ANTARES_A_RESOUDRE::X, so the caller is
// responsible for X holding the state it wants published.
void FillLegacySimulationTable(
  Antares::IO::Outputs::SimulationTable& simulationTable,
  PROBLEME_HEBDO& problemeHebdo,
  const Antares::LinearProblem::Api::FillContext& fillContext,
  const LegacyNameMapper& nameMapper,
  unsigned currentBlock);

// Block index of the week problemeHebdo currently holds. Same expression as the
// weekly branch of OPT_TryToCallSimplex, for callers that run once per week
// rather than once per optimisation interval.
unsigned LegacyWeeklyBlock(const PROBLEME_HEBDO& problemeHebdo);

// Publishes the legacy results as they stand *after* post-processing.
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
void DumpLegacySimulationTableAfterPostProcess(
  Antares::IO::Outputs::SimulationTable& simulationTable,
  PROBLEME_HEBDO& problemeHebdo,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
  unsigned currentBlock);

} // namespace Antares::Optimization
