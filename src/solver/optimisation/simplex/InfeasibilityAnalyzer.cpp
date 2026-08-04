// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/simplex/InfeasibilityAnalyzer.h"

#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "antares/solver/optimisation/LegacyOrtoolsLinearProblem.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/utils/mps_utils.h"
#include "antares/solver/utils/filename.h"
#include "antares/solver/optimisation/simplex/LpFiller.h"
#include "antares/solver/optimisation/simplex/SimplexResult.h"

using namespace Antares;
using namespace Antares::Optimization;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Solver;

namespace Antares::Solver::Optimization::Simplex
{

bool InfeasibilityAnalyzer::analyze(PROBLEME_HEBDO* problemeHebdo,
                                    const SimplexResult& originalResult,
                                    const SingleOptimOptions& options,
                                    int NumIntervalle,
                                    const OptPeriodStringGenerator& periodString,
                                    Solver::IResultWriter& writer,
                                    int optimizationNumber)
{
    const auto& modelerData = problemeHebdo->modelerData;
    bool hasModelerData = modelerData != nullptr;
    const bool isMip = problemeHebdo->OptimisationAvecVariablesEntieres;

    // Duplicate the LP filling logic for infeasibility analysis
    LegacyOrtoolsLinearProblem infeasibleProblem(isMip, options.solverName);
    FillContext fillCtx = LpFiller::buildFillContext(problemeHebdo, NumIntervalle);
    Optimisation::OptimEntityContainer optimEntityContainer(infeasibleProblem);
    LpFiller::fillLinearProblem(fillCtx, problemeHebdo, optimEntityContainer, nullptr);

    auto MPproblem = infeasibleProblem.getMpSolver();
    auto analyzer = Antares::Optimization::makeUnfeasiblePbAnalyzer();
    analyzer->run(MPproblem.get());
    analyzer->printReport();

    // Export MPS for error diagnostics (using original problem data)
    mpsWriterFactory mps_writer_factory(problemeHebdo->ExportMPS,
                                        problemeHebdo->exportMPSOnError,
                                        optimizationNumber,
                                        *originalResult.originalProblem);

    auto mps_writer_on_error = mps_writer_factory.createOnOptimizationError();
    const std::string filename = ::createMPSfilename(
      periodString,
      optimizationNumber);
    mps_writer_on_error->runIfNeeded(writer, filename);

    return false;
}

} // namespace Antares::Solver::Optimization::Simplex
