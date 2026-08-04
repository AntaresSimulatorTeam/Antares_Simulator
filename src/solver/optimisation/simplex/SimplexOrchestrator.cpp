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
#include "antares/solver/optimisation/LegacyExtraOutputs.h"
#include "antares/solver/optimisation/LegacyNameMapper.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/simplex/InfeasibilityAnalyzer.h"
#include "antares/solver/optimisation/simplex/LpFiller.h"
#include "antares/solver/optimisation/simplex/SimplexResult.h"
#include "antares/solver/utils/filename.h"
#include "antares/solver/utils/mps_utils.h"

using Antares::Constants::nbHoursInAWeek;
using Antares::Optimisation::BendersDecomposition;
using Antares::Optimisation::OptimEntityContainer;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimization::LegacyNameMapper;
using Antares::Optimization::LegacyOrtoolsLinearProblem;

using Antares::Solver::IResultWriter;
using Antares::Solver::Optimization::SingleOptimOptions;
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

void fillLegacySimulationTable(SimulationTable& simulationTable,
                               PROBLEME_HEBDO& problemeHebdo,
                               const FillContext& fillContext,
                               const LegacyNameMapper& nameMapper,
                               unsigned currentBlock)
{
    const PROBLEME_ANTARES_A_RESOUDRE& problem = *problemeHebdo.ProblemeAResoudre;

    // LegacyVariablesInfo, X and CoutLineaire are all sized to NombreDeVariables
    // in resizeProbleme, so the index-based reads below are always in bounds.
    assert(problem.LegacyVariablesInfo.size() == static_cast<std::size_t>(problem.NombreDeVariables)
           && problem.X.size() == static_cast<std::size_t>(problem.NombreDeVariables)
           && problem.CoutLineaire.size() == static_cast<std::size_t>(problem.NombreDeVariables));
    for (int index = 0; index < problem.NombreDeVariables; ++index)
    {
        const auto& info = problem.LegacyVariablesInfo[static_cast<std::size_t>(index)];
        if (!info)
        {
            continue;
        }

        simulationTable.addEntry(
          {.block = currentBlock,
           .component = info->component,
           .output = nameMapper.mapOutput(info->name),
           .absolute_time_index = info->timeIndex,
           .block_time_index = Antares::Optimization::LegacyBlockTimeIndex(fillContext,
                                                                           info->timeIndex),
           .scenario_index = fillContext.getYear(),
           .value = problem.X[static_cast<std::size_t>(index)],
           .status = std::nullopt});
    }

    Antares::Optimization::AddLegacyExtraOutputs(simulationTable,
                                                 problemeHebdo,
                                                 fillContext,
                                                 currentBlock);
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
                                         IO::Outputs::SimulationTable* simulationTable):
    options_(std::move(options)),
    problemeHebdo_(problemeHebdo),
    NumIntervalle_(NumIntervalle),
    optimizationNumber_(optimizationNumber),
    periodString_(periodString),
    writer_(writer),
    simulationTable_(simulationTable)
{
}

SimplexResult SimplexOrchestrator::solve()
{
    const auto& ProblemeAResoudre = problemeHebdo_.ProblemeAResoudre;

    const int opt = optimizationNumber_ - 1;
    assert(opt >= 0 && opt < 2);
    OptimizationStatistics& optimizationStatistics = problemeHebdo_.optimizationStatistics[opt];

    const auto& modelerData = problemeHebdo_.modelerData;
    const bool isMip = problemeHebdo_.OptimisationAvecVariablesEntieres;

    // Step 1: Create LP problem
    ortoolsProblem_ = std::make_shared<LegacyOrtoolsLinearProblem>(isMip, options_.solverName);

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
    optimEntityContainer_ = std::make_unique<OptimEntityContainer>(*ortoolsProblem_);

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
    fillLegacySimulationTable(*simulationTable_,
                              problemeHebdo_,
                              *fillCtx_,
                              legacyNameMapper,
                              currentBlock);

    measure_.tick();
    timeMeasure_.simulationTableFillTime = measure_.duration_ms();
}

} // namespace Antares::Solver::Optimization::Simplex
