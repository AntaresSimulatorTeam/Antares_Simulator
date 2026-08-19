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

} // namespace Antares::Optimization
