// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/simplex/SimplexOrchestrator.h"

#include <cassert>
#include <spx_constantes_externes.h>

#include <antares/antares/constants.h>
#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include <antares/solver/utils/ortools_utils.h>
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/convertOrtoolsBasisStatus.h"
#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "antares/solver/optimisation/LegacyNameMapper.h"
#include "antares/solver/optimisation/LegacySimulationTableSnapshot.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/ortools_wrapper.h"
#include "antares/solver/optimisation/simplex/InfeasibilityAnalyzer.h"
#include "antares/solver/optimisation/simplex/LpFiller.h"
#include "antares/solver/optimisation/simplex/SimplexResult.h"
#include "antares/solver/utils/filename.h"
#include "antares/solver/utils/mps_utils.h"

using Antares::Constants::nbHoursInAWeek;
using Antares::LinearProblem::BendersDecomposition;
using Antares::LinearProblem::OptimEntityContainer;
using Antares::LinearProblem::Api::FillContext;
using Antares::Optimization::LegacyNameMapper;
using Antares::Optimization::LegacyOrtoolsLinearProblem;

using Antares::Solver::IResultWriter;
using Antares::Optimization::SingleOptimOptions;
using MPSolver = operations_research::MPSolver;
using SimulationTable = IO::Outputs::SimulationTable;

namespace
{
static std::once_flag logProblemSizeFlag;

static void logProblemSizeOnce(const MPSolver* mpSolver)
{
    logs.info();
    logs.info();
    logs.info() << " Total Problem size : " << mpSolver->NumVariables() << " variables, "
                << mpSolver->NumConstraints() << " constraints";
    logs.info();
    logs.info();
}
} // namespace

namespace Antares::Solver::Optimization::Simplex
{

SimplexOrchestrator::SimplexOrchestrator(SingleOptimOptions options,
                                         PROBLEME_HEBDO& problemeHebdo,
                                         int NumIntervalle,
                                         int optimizationNumber,
                                         const OptPeriodStringGenerator& periodString,
                                         Solver::IResultWriter& writer,
                                         IO::Outputs::SimulationTable* simulationTable,
                                         const Antares::Optimization::InactiveComponentsAnalyzer*
                                           inactiveComponents):
    options_(std::move(options)),
    problemeHebdo_(problemeHebdo),
    NumIntervalle_(NumIntervalle),
    optimizationNumber_(optimizationNumber),
    periodString_(periodString),
    writer_(writer),
    simulationTable_(simulationTable),
    inactiveComponents_(inactiveComponents)
{
}

SimplexResult SimplexOrchestrator::solve()
{
    const auto& ProblemeAResoudre = problemeHebdo_.ProblemeAResoudre;

    const int opt = optimizationNumber_ - 1;
    assert(opt >= 0 && opt < 2);
    OptimizationStatistics& optimizationStatistics = problemeHebdo_.optimizationStatistics[opt];

    const bool isMip = problemeHebdo_.OptimisationAvecVariablesEntieres;

    // Release the problem retained by the previous pass before building this
    // one, so the two never coexist: this function's peak is one problem, not
    // two. Nothing can still want the old one -- a post-process dump runs after
    // the week's last pass, and a week whose solve fails throws out of
    // OPT_OptimisationHebdomadaireLineaire before the post-processes run, so the
    // value cleared here could never have been published either way.
    problemeHebdo_.lastSolvedModelerProblem.reset();

    // Step 1: Create LP problem
    ortoolsProblem_ = std::make_shared<LegacyOrtoolsLinearProblem>(isMip, options_.solverName);
    // Heap-allocated so it can outlive this call: a post-process simulation
    // table re-emits the modeler rows through it, long after the solve.
    problemeHebdo_.ortoolsProblem_ = ortoolsProblem_;

    // Step 2: Fill LP
    createAndFillLp();

    // Step 3: Store solver reference
    solver_ = ortoolsProblem_->getMpSolver();
    ProblemeAResoudre->ProblemesSpx[NumIntervalle_] = solver_;

    // Step 4: Export MPS
    exportMps();

    // Step 5: Solve with timing
    measure_.tick();
    timeMeasure_.updateTime = measure_.duration_ms();
    optimizationStatistics.addUpdateTime(timeMeasure_.updateTime);
    measure_.reset();

    ORTOOLS_Simplexe(ProblemeAResoudre.get(), solver_.get(), options_);

    measure_.tick();
    logs.info() << fmt::format("Problem {}-{} solved in {}",
                               problemeHebdo_.year,
                               problemeHebdo_.weekInTheYear,
                               measure_.toStringInSeconds());
    timeMeasure_.solveTime = measure_.duration_ms();
    optimizationStatistics.addSolveTime(timeMeasure_.solveTime);

    // Step 6: Handle result
    if (!handleSolve())
    {
        return {.timeMeasure = timeMeasure_,
                .originalProblem = ortoolsProblem_,
                .objectiveValue = 0,
                .success = false};
    }

    // Hand the modeler side to the post-process dumps. Called for both passes,
    // so what survives is the last one actually run. Independent of
    // `simulationTable`: the stage selection can leave this pass without a table
    // of its own while a later post-process stage still needs these rows.
    if (problemeHebdo_.retainSolvedModelerProblem)
    {
        problemeHebdo_.lastSolvedModelerProblem = std::make_shared<
          const Antares::Optimization::SolvedModelerProblem>(
          Antares::Optimization::SolvedModelerProblem{
            .problem = ortoolsProblem_,
            .entities = problemeHebdo_.optimEntityContainer,
            .objectiveValue = ::getObjectiveValue(solver_.get())});
    }

    // Success path: fill simulation table
    if (simulationTable_)
    {
        fillSimulationTable();
    }

    return {.timeMeasure = timeMeasure_,
            .originalProblem = ortoolsProblem_,
            .objectiveValue = ::getObjectiveValue(solver_.get()),
            .success = true};
}

bool SimplexOrchestrator::handleSolve() const
{
    const auto& ProblemeAResoudre = problemeHebdo_.ProblemeAResoudre;
    if (ProblemeAResoudre->ExistenceDUneSolution != OUI_SPX)
    {
        if (ProblemeAResoudre->ExistenceDUneSolution != SPX_ERREUR_INTERNE)
        {
            if (solver_)
            {
                ProblemeAResoudre->ProblemesSpx[NumIntervalle_].reset();
            }

            logs.info() << " Solver: resolution failed";
            logs.debug() << " solver: resetting";
            return false;
        }
        throw Antares::FatalError("Internal error: insufficient memory");
    }
    return true;
}

void SimplexOrchestrator::createAndFillLp()
{
    fillCtx_ = LpFiller::buildFillContext(problemeHebdo_, NumIntervalle_);
    const auto& modelerData = problemeHebdo_.modelerData;
    bool hasModelerData = modelerData != nullptr;
    // Kept alive on problemeHebdo_ past this call: a post-process simulation
    // table re-emits the modeler rows through it, long after the solve.
    problemeHebdo_.optimEntityContainer = std::make_shared<OptimEntityContainer>(*ortoolsProblem_);
    optimEntityContainer_ = problemeHebdo_.optimEntityContainer;

    BendersDecomposition* bendersDecomposition = hasModelerData ? &modelerData->bendersDecomposition
                                                                : nullptr;

    LpFiller::fillLinearProblem(*fillCtx_,
                                problemeHebdo_,
                                *optimEntityContainer_,
                                bendersDecomposition);
}

void SimplexOrchestrator::exportMps()
{
    std::call_once(logProblemSizeFlag, logProblemSizeOnce, solver_.get());

    const std::string mpsFilename = ::createMPSfilename(periodString_, optimizationNumber_);

    mpsWriterFactory mps_writer_factory(problemeHebdo_.ExportMPS,
                                        problemeHebdo_.exportMPSOnError,
                                        optimizationNumber_,
                                        *ortoolsProblem_);

    auto mps_writer = mps_writer_factory.create(problemeHebdo_.NamedProblems);
    mps_writer->runIfNeeded(writer_, mpsFilename);
}

void SimplexOrchestrator::fillSimulationTable()
{
    IO::Outputs::TimeConversionMode timeConversionMode
      = problemeHebdo_.OptimisationAuPasHebdomadaire ? IO::Outputs::TimeConversionMode::WeeklyBlocks
                                                     : IO::Outputs::TimeConversionMode::DailyBlocks;

    // Compute the current block index (weekly blocks if optimization is weekly,
    // daily blocks otherwise).
    unsigned currentBlock;
    const unsigned heure = static_cast<unsigned>(problemeHebdo_.HeureDansLAnnee);
    if (problemeHebdo_.OptimisationAuPasHebdomadaire)
    {
        currentBlock = heure / nbHoursInAWeek;
    }
    else
    {
        currentBlock = heure / HOURS_PER_DAY + static_cast<unsigned>(NumIntervalle_);
    }

    if (problemeHebdo_.modelerData)
    {
        IO::Outputs::FillSimulationTable(*simulationTable_,
                                         *ortoolsProblem_,
                                         ::getObjectiveValue(solver_.get()),
                                         *problemeHebdo_.modelerData,
                                         *optimEntityContainer_,
                                         *fillCtx_,
                                         currentBlock,
                                         timeConversionMode,
                                         true);
    }

    static constexpr LegacyNameMapper legacyNameMapper;
    const auto& ProblemeAResoudre = problemeHebdo_.ProblemeAResoudre;
    Antares::Optimization::FillLegacySimulationTable(
      *simulationTable_,
      problemeHebdo_,
      {ProblemeAResoudre->X, ProblemeAResoudre->CoutsMarginauxDesContraintes},
      *fillCtx_,
      legacyNameMapper,
      currentBlock,
      inactiveComponents_);

    // Hand the modeler side to the post-process dumps. Called for both
    // passes, so what survives is the last one actually run.
    problemeHebdo_.lastSolvedModelerProblem = std::make_shared<
      const Antares::Optimization::SolvedModelerProblem>(
      Antares::Optimization::SolvedModelerProblem{
        .problem = ortoolsProblem_,
        .entities = problemeHebdo_.optimEntityContainer,
        .objectiveValue = ::getObjectiveValue(solver_.get())});

    measure_.tick();
    timeMeasure_.simulationTableFillTime = measure_.duration_ms();
}

} // namespace Antares::Solver::Optimization::Simplex
