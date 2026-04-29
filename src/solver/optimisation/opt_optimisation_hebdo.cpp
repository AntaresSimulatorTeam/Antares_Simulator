// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/antares/fatal-error.h>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/logs/logs.h>
#include "antares/io/outputs/ISimulationTable.h"
#include "antares/solver/optimisation/OptimisationsSimulationTable.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data;

using Solver::Optimization::OptimizationOptions;
using Solver::Optimization::SingleOptimOptions;

bool OPT_PilotageOptimisationLineaire(const OptimizationOptions&,
                                      PROBLEME_HEBDO*,
                                      Solver::IResultWriter&,
                                      Solver::Simulation::ISimulationObserver&,
                                      OptimisationsSimulationTable* simulationTables);
bool OPT_PilotageOptimisationQuadratique(const SingleOptimOptions&, PROBLEME_HEBDO*);

void OPT_OptimisationHebdomadaireLineaire(
  const OptimizationOptions& options,
  PROBLEME_HEBDO* pProblemeHebdo,
  Solver::IResultWriter& writer,
  Solver::Simulation::ISimulationObserver& simulationObserver,
  OptimisationsSimulationTable* simulationTables)
{
    if (!OPT_PilotageOptimisationLineaire(options,
                                          pProblemeHebdo,
                                          writer,
                                          simulationObserver,
                                          simulationTables))
    {
        logs.error() << "Linear optimization failed";
        throw UnfeasibleProblemError("Linear optimization failed");
    }
}

void OPT_OptimisationHebdomadaireQuadratique(const OptimizationOptions& options,
                                             PROBLEME_HEBDO* pProblemeHebdo)

{
    if (!OPT_PilotageOptimisationQuadratique(options.quadraticOptimOptions, pProblemeHebdo))
    {
        logs.error() << "Quadratic optimization failed";
        throw UnfeasibleProblemError("Quadratic optimization failed");
    }
}
