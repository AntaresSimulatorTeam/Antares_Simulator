// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/simplex/InfeasibilityAnalyzer.h"

#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/utils/mps_utils.h"
#include "antares/solver/utils/filename.h"
#include "antares/solver/optimisation/simplex/LpFiller.h"

using Antares::Optimization::LegacyOrtoolsLinearProblem;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimisation::OptimEntityContainer;
using Antares::Solver::IResultWriter;

namespace Antares::Solver::Optimization::Simplex
{

bool InfeasibilityAnalyzer::analyze(PROBLEME_HEBDO* problemeHebdo,
                                    const std::shared_ptr<Antares::Optimization::LegacyOrtoolsLinearProblem>& originalProblem,
                                    const SingleOptimOptions& options,
                                    int NumIntervalle,
                                    const OptPeriodStringGenerator& periodString,
                                    IResultWriter& writer,
                                    int optimizationNumber)
{
    const bool isMip = problemeHebdo->OptimisationAvecVariablesEntieres;

    // Duplicate the LP filling logic for infeasibility analysis
    LegacyOrtoolsLinearProblem infeasibleProblem(isMip, options.solverName);
    FillContext fillCtx = LpFiller::buildFillContext(problemeHebdo, NumIntervalle);
    OptimEntityContainer optimEntityContainer(infeasibleProblem);
    LpFiller::fillLinearProblem(fillCtx, problemeHebdo, optimEntityContainer, nullptr);

    auto MPproblem = infeasibleProblem.getMpSolver();
    auto analyzer = Antares::Optimization::makeUnfeasiblePbAnalyzer();
    analyzer->run(MPproblem.get());
    analyzer->printReport();

    // Export MPS for error diagnostics (using original problem data)
    mpsWriterFactory mps_writer_factory(
      problemeHebdo->ExportMPS,
      problemeHebdo->exportMPSOnError,
      optimizationNumber,
      *originalProblem);

    auto mps_writer_on_error = mps_writer_factory.createOnOptimizationError();
    const std::string filename = ::createMPSfilename(
      periodString,
      optimizationNumber);
    mps_writer_on_error->runIfNeeded(writer, filename);

    return false;
}

} // namespace Antares::Solver::Optimization::Simplex
