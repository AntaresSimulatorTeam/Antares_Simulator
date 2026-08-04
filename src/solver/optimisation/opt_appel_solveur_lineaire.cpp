// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cassert>

#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include "antares/optimization-options/options.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/utils/opt_period_string_generator.h"
#include "antares/writer/i_writer.h"
#include "antares/io/outputs/SimulationTable.h"
#include "antares/solver/optimisation/simplex/SimplexOrchestrator.h"
#include "antares/solver/optimisation/simplex/SimplexResult.h"
#include "antares/solver/optimisation/simplex/InfeasibilityAnalyzer.h"
#include "antares/solver/optimisation/simplex/LpFiller.h"

using namespace Antares::Solver;
using namespace Antares::Solver::Optimization;
using namespace Antares::Solver::Optimization::Simplex;

namespace Antares::Solver::Optimization
{

bool OPT_AppelDuSimplexe(const SingleOptimOptions& options,
                         PROBLEME_HEBDO* problemeHebdo,
                         int NumIntervalle,
                         int optimizationNumber,
                         const OptPeriodStringGenerator& optPeriodStringGenerator,
                         IResultWriter& writer,
                         IO::Outputs::SimulationTable* simulationTable)
{
    SimplexOrchestrator orchestrator(options,
                                     problemeHebdo,
                                     NumIntervalle,
                                     optimizationNumber,
                                     &optPeriodStringGenerator,
                                     writer,
                                     simulationTable);
    auto result = orchestrator.solve();

    if (result.success)
    {
        Simplex::applyResults(problemeHebdo, result, NumIntervalle, optimizationNumber);
        return true;
    }
    else
    {
        return InfeasibilityAnalyzer::analyze(problemeHebdo,
                                              result,
                                              options,
                                              NumIntervalle,
                                              optPeriodStringGenerator,
                                              writer,
                                              optimizationNumber);
    }
}

} // namespace Antares::Solver::Optimization

// ──────────────────────────────────────────────────────────────
// Backward-compatible wrappers for legacy callers (e.g. api/)
// These delegate to the new Simplex::LpFiller implementation.
// ──────────────────────────────────────────────────────────────
namespace
{
Optimisation::LinearProblemApi::FillContext
buildFillContextWrapper(const PROBLEME_HEBDO* problemeHebdo, int NumIntervalle)
{
    return LpFiller::buildFillContext(problemeHebdo, NumIntervalle);
}

void
fillLinearProblemWrapper(const Optimisation::LinearProblemApi::FillContext& fillCtx,
                         PROBLEME_HEBDO* problemeHebdo,
                         Optimisation::OptimEntityContainer& optimEntityContainer,
                         Optimisation::BendersDecomposition* bendersDecomposition)
{
    LpFiller::fillLinearProblem(fillCtx, problemeHebdo, optimEntityContainer, bendersDecomposition);
}
} // namespace

// Namespace-scope wrappers matching the original declarations in opt_fonctions.h
Optimisation::LinearProblemApi::FillContext buildFillContext(const PROBLEME_HEBDO* problemeHebdo,
                                                             int NumIntervalle)
{
    return buildFillContextWrapper(problemeHebdo, NumIntervalle);
}

void fillLinearProblem(const Optimisation::LinearProblemApi::FillContext& fillCtx,
                       PROBLEME_HEBDO* problemeHebdo,
                       Optimisation::OptimEntityContainer& optimEntityContainer,
                       Optimisation::BendersDecomposition* bendersDecomposition)
{
    fillLinearProblemWrapper(fillCtx, problemeHebdo, optimEntityContainer, bendersDecomposition);
}
