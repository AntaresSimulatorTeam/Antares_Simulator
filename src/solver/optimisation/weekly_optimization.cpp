// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/weekly_optimization.h"

#include "antares/solver/optimisation/opt_fonctions.h"

namespace Antares::Solver::Optimization
{
WeeklyOptimization::WeeklyOptimization(const OptimizationOptions& options,
                                       PROBLEME_HEBDO* problemeHebdo,
                                       IResultWriter& writer,
                                       Simulation::ISimulationObserver& simulationObserver,
                                       OptimisationsSimulationTable* simulationTables):
    options_(options),
    problemeHebdo_(problemeHebdo),
    writer_(writer),
    simulationObserver_(simulationObserver),
    simulationTables_(simulationTables)
{
}

void WeeklyOptimization::solve()
{
    OPT_OptimisationHebdomadaireLineaire(options_,
                                         problemeHebdo_,
                                         writer_,
                                         simulationObserver_.get(),
                                         simulationTables_);
}

} // namespace Antares::Solver::Optimization
