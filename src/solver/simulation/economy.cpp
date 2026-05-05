// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/economy.h"

#ifdef ANTARES_WEEK_PARALLELISM
#include <future>
#include <semaphore>
#endif

#include <antares/exception/AssertionError.hpp>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"
#ifdef ANTARES_WEEK_PARALLELISM
#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"
#endif
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/solver_utils.h"

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Simulation
{
Economy::Economy(Data::Study& study,
                 IResultWriter& resultWriter,
                 Simulation::ISimulationObserver& simulationObserver):
    study(study),
    preproOnly(false),
    resultWriter_(resultWriter),
    simulationObserver_(simulationObserver),
    simulationTables_(study.parameters.noOutput ? 0 : study.maxNbYearsInParallel)
{
}

Benchmarking::OptimizationInfo Economy::getOptimizationInfo() const
{
    const uint numSpace = 0;
    const auto& Pb = pProblemesHebdo[numSpace].ProblemeAResoudre;
    Benchmarking::OptimizationInfo optInfo;

    optInfo.nbVariables = Pb->NombreDeVariables;
    optInfo.nbConstraints = Pb->NombreDeContraintes;
    return optInfo;
}

void Economy::setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel)
{
    pNbMaxPerformedYearsInParallel = nbMaxPerformedYearsInParallel;
}

void Economy::initializeState(Variable::State& state, uint numSpace)
{
    state.problemeHebdo = &pProblemesHebdo[numSpace];
    state.numSpace = numSpace;
}

OptimisationsSimulationTable& Economy::getSimulationTable(uint numSpace)
{
    if (numSpace >= simulationTables_.size())
    {
        throw std::out_of_range("Error: there is no simulation table for numSpace: "
                                + std::to_string(numSpace));
    }
    return simulationTables_[numSpace];
}

std::string Economy::getSimulationTableHeader() const
{
    if (!simulationTables_.empty())
    {
        return simulationTables_.at(0).getHeader();
    }
    return "";
}

bool Economy::simulationBegin()
{
#ifdef ANTARES_WEEK_PARALLELISM
    pNbMaxWeeksInParallel = study.maxNbWeeksInParallel;
    if (pNbMaxWeeksInParallel < 1)
    {
        pNbMaxWeeksInParallel = 1;
    }

    if (pNbMaxWeeksInParallel > 1)
    {
        logs.info() << "Week-level parallelism enabled: " << pNbMaxWeeksInParallel
                    << " weeks in parallel per year (Fast Mode UC assumed).";
        if (study.parameters.unitCommitment.ucMode != Data::UnitCommitmentMode::ucHeuristicFast)
        {
            logs.warning()
              << "Week-level parallelism is designed for Fast Mode UC. Results may differ "
                 "from the sequential solve when other UC modes are used.";
        }
    }
#else
    pNbMaxWeeksInParallel = 1;
#endif

    if (!preproOnly)
    {
        pProblemesHebdo.resize(pNbMaxPerformedYearsInParallel);
        weeklyOptProblems_.clear();
        postProcessesList_.resize(pNbMaxPerformedYearsInParallel);

        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            SIM_InitialisationProblemeHebdo(study,
                                            pProblemesHebdo[numSpace],
                                            nbHoursInAWeek,
                                            numSpace);
            auto* simulationsTables = simulationTables_.empty() ? nullptr
                                                                : &simulationTables_[numSpace];
            weeklyOptProblems_.emplace_back(study.parameters.optOptions,
                                            &pProblemesHebdo[numSpace],
                                            resultWriter_,
                                            simulationObserver_.get(),
                                            simulationsTables);

            postProcessesList_[numSpace] = interfacePostProcessList::create(
              study.parameters.adqPatchParams,
              &pProblemesHebdo[numSpace],
              numSpace,
              study.areas,
              study.parameters,
              study.calendar,
              resultWriter_);
        }
    }

    pStartTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    pNbWeeks = study.parameters.simulationDays.numberOfWeeks();
    return true;
}

#ifdef ANTARES_WEEK_PARALLELISM
// ---------------------------------------------------------------------------
// Internal helper: result of solving a single week's LP problem.
// The solved PROBLEME_HEBDO is stored in a unique_ptr so the aggregation loop
// can call variables.hourForEachArea(state, numSpace) with the correct data.
// unique_ptr is used because PROBLEME_HEBDO is not move-assignable (its
// OptimizationStatistics member has deleted move assignment).
// ---------------------------------------------------------------------------
struct WeekSolveResult
{
    bool success = true;
    bool stopSimulation = false; // true if an error requires halting the year
    std::string assertionErrorMsg;
    std::unique_ptr<PROBLEME_HEBDO> solvedProblem; // owns the clone after solve
    TIME_MEASURES timeMeasure;
    int hourInTheYear = 0; // hour at the start of this week
    uint weekIndex = 0;
};

// ---------------------------------------------------------------------------
// Solve one week: set up the problem, run the LP, run post-processes.
// This function does NOT touch Variable::State (no weekBegin / hourForEachArea
// / weekEnd / weekForEachArea calls) so it is safe to call from async threads.
//
// \param problem    The week's PROBLEME_HEBDO clone, already set up with the
//                   correct previousSimulationFinalLevel. Modified in place by
//                   SIM_RenseignementProblemeHebdo, BuildThermalPart, and solve().
// \param weekOpt    WeeklyOptimization whose problemeHebdo_ points to \p problem.
// \param postProcessList PostProcessList whose problemeHebdo_ points to \p problem.
// ---------------------------------------------------------------------------
static WeekSolveResult solveOneWeek(Data::Study& study,
                                    PROBLEME_HEBDO& problem,
                                    uint weekIndex,
                                    int hourInTheYear,
                                    const HYDRO_VENTILATION_RESULTS& hydroVentilationResults,
                                    const yearRandomNumbers& randomForYear,
                                    const Antares::Data::Area::ScratchMap& scratchmap,
                                    Optimization::WeeklyOptimization& weekOpt,
                                    interfacePostProcessList& postProcessList)
{
    WeekSolveResult res;
    res.weekIndex = weekIndex;
    res.hourInTheYear = hourInTheYear;

    problem.weekInTheYear = weekIndex;
    problem.HeureDansLAnnee = static_cast<uint32_t>(hourInTheYear);

    ::SIM_RenseignementProblemeHebdo(study,
                                     problem,
                                     weekIndex,
                                     hourInTheYear,
                                     hydroVentilationResults,
                                     scratchmap);

    BuildThermalPartOfWeeklyProblem(study,
                                    problem,
                                    hourInTheYear,
                                    randomForYear.pThermalNoisesByArea,
                                    problem.year);
    try
    {
        weekOpt.solve();

        optRuntimeData opt_runtime_data(problem.year, weekIndex, hourInTheYear);
        postProcessList.runAll(opt_runtime_data);

        res.timeMeasure = problem.timeMeasure;
        // Store the solved problem via unique_ptr (PROBLEME_HEBDO is not move-assignable
        // because OptimizationStatistics has deleted move assignment).
        res.solvedProblem = std::make_unique<PROBLEME_HEBDO>(std::move(problem));
        res.success = true;
    }
    catch (Data::AssertionError& ex)
    {
        res.success = false;
        res.stopSimulation = true;
        res.assertionErrorMsg = ex.what();
    }
    catch (Data::UnfeasibleProblemError&)
    {
        res.success = false;
        res.stopSimulation = Data::stopSimulation(
          study.parameters.include.unfeasibleProblemBehavior);
    }
    return res;
}
#endif // ANTARES_WEEK_PARALLELISM

bool Economy::year(Progression::Task& progression,
                   Variable::State& state,
                   uint numSpace,
                   yearRandomNumbers& randomForYear,
                   std::list<uint>& failedWeekList,
                   const HYDRO_VENTILATION_RESULTS& hydroVentilationResults,
                   OptimizationStatisticsWriter& optWriter,
                   Benchmarking::DurationCollector& durationCollector,
                   const Antares::Data::Area::ScratchMap& scratchmap)
{
    /*
     * Week-level parallelism notes (pNbMaxWeeksInParallel > 1):
     * (Only active when compiled with ANTARES_WEEK_PARALLELISM defined.)
     *
     *   WHY WEEKS ARE PARALLELISABLE IN FAST MODE UC
     *   In Fast Mode (heuristic UC), there are no cross-week unit-commitment
     *   constraints (e.g. minimum up/down times spanning week boundaries).
     *   Each week's LP is therefore fully self-contained once the initial hydro
     *   reservoir level is known.
     *
     *   HYDRO APPROXIMATION
     *   The only true cross-week dependency is the reservoir carry-forward.
     *   precomputeWeeklyInitialHydroLevels() approximates week w's initial level
     *   from the ventilation-provided daily levels.  For areas where
     *   TurbinageEntreBornes = false (the vast majority), these levels are
     *   identical to what SIM_RenseignementProblemeHebdo would use, so there is
     *   zero approximation error.  For TurbinageEntreBornes = true areas, the
     *   ventilation end-of-week levels are used instead of the LP-derived ones,
     *   introducing a bounded error acceptable in Fast Mode.
     *
     *   VARIABLE::STATE WRITES ARE DEFERRED
     *   variables.weekBegin / hourForEachArea / weekEnd / weekForEachArea write
     *   into the shared Variable::State object (one per numSpace).  They are
     *   called ONLY in the sequential aggregation loop below, never from inside
     *   async lambdas.
     *
     *   DEADLOCK AVOIDANCE
     *   We use std::async (which spawns new OS threads) rather than the existing
     *   pQueueService Yuni thread pool.  Reusing pQueueService would deadlock
     *   because week tasks would wait for slots currently held by their own
     *   yearJob.
     */

    (void)progression; // progression is advanced by Economy::incrementProgression() after year()
    // No failed week at year start
    failedWeekList.clear();
    auto& currentProblem = pProblemesHebdo[numSpace];
    currentProblem.year = state.year;

    PrepareRandomNumbers(study, currentProblem, randomForYear);
    SetInitialHydroLevel(study, currentProblem, hydroVentilationResults);

    state.startANewYear();

    int hourInTheYear = pStartTime;

    // In order to avoid slight differences in parallel/sequential, we clear the basis at the start
    // of each year
    currentProblem.ProblemeAResoudre->clearBasis();

    // -------------------------------------------------------------------------
    // SEQUENTIAL PATH (pNbMaxWeeksInParallel == 1) — original behaviour, untouched
    // -------------------------------------------------------------------------
#ifdef ANTARES_WEEK_PARALLELISM
    if (pNbMaxWeeksInParallel <= 1)
#else
    if (true) // parallel branch compiled out: always sequential
#endif
    {
        for (uint w = 0; w != pNbWeeks; ++w)
        {
            state.hourInTheYear = hourInTheYear;
            currentProblem.weekInTheYear = state.weekInTheYear = w;
            currentProblem.HeureDansLAnnee = hourInTheYear;

            ::SIM_RenseignementProblemeHebdo(study,
                                             currentProblem,
                                             state.weekInTheYear,
                                             hourInTheYear,
                                             hydroVentilationResults,
                                             scratchmap);

            BuildThermalPartOfWeeklyProblem(study,
                                            currentProblem,
                                            hourInTheYear,
                                            randomForYear.pThermalNoisesByArea,
                                            state.year);
            auto* currentSimTable = simulationTables_.empty() ? nullptr
                                                              : &simulationTables_[numSpace];
            try
            {
                weeklyOptProblems_[numSpace].solve();
                if (currentSimTable)
                {
                    currentSimTable->write();
                }
                // Runs all the post processes in the list of post-process commands
                optRuntimeData opt_runtime_data(state.year, w, hourInTheYear);
                postProcessesList_[numSpace]->runAll(opt_runtime_data);

                variables.weekBegin(state);
                uint previousHourInTheYear = state.hourInTheYear;

                for (uint hw = 0; hw != nbHoursInAWeek;
                     ++hw, ++state.hourInTheYear, ++state.hourInTheSimulation)
                {
                    state.hourInTheWeek = hw;

                    state.ntc = currentProblem.ValeursDeNTC[hw];

                    variables.hourBegin(state.hourInTheYear);

                    variables.hourForEachArea(state, numSpace);

                    variables.hourEnd(state, state.hourInTheYear);
                }

                state.hourInTheYear = previousHourInTheYear;
                variables.weekForEachArea(state, numSpace);
                variables.weekEnd(state);

                for (int opt = 0; opt < 7; opt++)
                {
                    state.optimalSolutionCost1 += currentProblem.coutOptimalSolution1[opt];
                    state.optimalSolutionCost2 += currentProblem.coutOptimalSolution2[opt];
                }
                optWriter.addTime(w, currentProblem.timeMeasure);
                addTimeMeasure(durationCollector, currentProblem.timeMeasure);
            }
            catch (Data::AssertionError& ex)
            {
                failedWeekList.push_back(w + 1);
                logs.error("Assertion error for week " + std::to_string(w + 1)
                           + " simulation is stopped : " + ex.what());
                return false;
            }
            catch (Data::UnfeasibleProblemError&)
            {
                failedWeekList.push_back(w + 1);
                if (Data::stopSimulation(study.parameters.include.unfeasibleProblemBehavior))
                {
                    return false;
                }
            }

            hourInTheYear += nbHoursInAWeek;
        }
    }
    // -------------------------------------------------------------------------
    // PARALLEL WEEK PATH (pNbMaxWeeksInParallel > 1)
    // Only compiled when ANTARES_WEEK_PARALLELISM is defined (i.e. cmake option
    // ENABLE_WEEK_PARALLELISM=ON, which is the default).
    // -------------------------------------------------------------------------
#ifdef ANTARES_WEEK_PARALLELISM
    else
    {
        // Step 1 – Precompute initial hydro levels for all weeks.
        auto weeklyLevels = precomputeWeeklyInitialHydroLevels(study.areas,
                                                                currentProblem,
                                                                hydroVentilationResults,
                                                                study.calendar,
                                                                pNbWeeks,
                                                                pStartTime);

        // Step 2 – Launch all week solves asynchronously, throttled by a semaphore.
        // std::counting_semaphore<N> requires a compile-time maximum; 52 is the
        // largest possible number of weeks in a year.
        static constexpr std::ptrdiff_t kMaxWeeks = 52;
        std::counting_semaphore<kMaxWeeks> sem(
          static_cast<std::ptrdiff_t>(pNbMaxWeeksInParallel));

        // We create a WeeklyOptimization and a postProcessList per week-clone so
        // each LP solve is fully independent.
        std::vector<std::future<WeekSolveResult>> futures;
        futures.reserve(pNbWeeks);

        int wHourInYear = pStartTime;
        for (uint w = 0; w < pNbWeeks; ++w)
        {
            // Clone the problem with the precomputed initial level for this week
            PROBLEME_HEBDO weekProblem = cloneProblemHebdoForWeek(currentProblem,
                                                                   w,
                                                                   weeklyLevels[w]);

            const int capturedHour = wHourInYear;
            const uint capturedW = w;

            // Capture all parameters needed to create WeeklyOptimization and
            // postProcessList INSIDE the lambda (after the problem is moved into
            // its final memory location).
            futures.push_back(std::async(
              std::launch::async,
              [this,
               capturedW,
               capturedHour,
               &hydroVentilationResults,
               &randomForYear,
               &scratchmap,
               &sem,
               numSpace,
               problem = std::move(weekProblem)]() mutable -> WeekSolveResult {
                  // Create WeeklyOptimization and postProcessList inside the lambda so
                  // their internal PROBLEME_HEBDO* pointers reference the moved-in clone.
                  Optimization::WeeklyOptimization weekOpt(study.parameters.optOptions,
                                                           &problem,
                                                           resultWriter_,
                                                           simulationObserver_.get(),
                                                           nullptr);
                  auto weekPostProc = interfacePostProcessList::create(
                    study.parameters.adqPatchParams,
                    &problem,
                    numSpace,
                    study.areas,
                    study.parameters,
                    study.calendar,
                    resultWriter_);

                  sem.acquire();
                  auto result = solveOneWeek(study,
                                             problem,
                                             capturedW,
                                             capturedHour,
                                             hydroVentilationResults,
                                             randomForYear,
                                             scratchmap,
                                             weekOpt,
                                             *weekPostProc);
                  sem.release();
                  return result;
              }));

            wHourInYear += static_cast<int>(nbHoursInAWeek);
        }

        // Step 3 – Sequential aggregation in week order.
        // Variable::State writes (weekBegin / hourForEachArea / weekEnd / weekForEachArea)
        // must happen here, NOT inside the async lambdas.
        for (uint w = 0; w < pNbWeeks; ++w)
        {
            WeekSolveResult res = futures[w].get();

            if (!res.success)
            {
                failedWeekList.push_back(w + 1);
                if (res.stopSimulation)
                {
                    if (!res.assertionErrorMsg.empty())
                    {
                        logs.error("Assertion error for week " + std::to_string(w + 1)
                                   + " simulation is stopped : " + res.assertionErrorMsg);
                    }
                    return false;
                }
                continue;
            }

            // Point state.problemeHebdo at this week's solved clone so that
            // variables.hourForEachArea etc. read the correct data.
            assert(res.solvedProblem != nullptr);
            PROBLEME_HEBDO& solvedPb = *res.solvedProblem;
            state.problemeHebdo = &solvedPb;
            state.hourInTheYear = res.hourInTheYear;
            state.weekInTheYear = w;

            variables.weekBegin(state);
            uint previousHourInTheYear = state.hourInTheYear;

            for (uint hw = 0; hw != nbHoursInAWeek;
                 ++hw, ++state.hourInTheYear, ++state.hourInTheSimulation)
            {
                state.hourInTheWeek = hw;
                state.ntc = solvedPb.ValeursDeNTC[hw];
                variables.hourBegin(state.hourInTheYear);
                variables.hourForEachArea(state, numSpace);
                variables.hourEnd(state, state.hourInTheYear);
            }

            state.hourInTheYear = previousHourInTheYear;
            variables.weekForEachArea(state, numSpace);
            variables.weekEnd(state);

            for (int opt = 0; opt < 7; opt++)
            {
                state.optimalSolutionCost1 += solvedPb.coutOptimalSolution1[opt];
                state.optimalSolutionCost2 += solvedPb.coutOptimalSolution2[opt];
            }
            optWriter.addTime(w, res.timeMeasure);
            addTimeMeasure(durationCollector, res.timeMeasure);
        }

        // Restore state.problemeHebdo to the canonical slot for this numSpace
        state.problemeHebdo = &currentProblem;
    }
#endif // ANTARES_WEEK_PARALLELISM

    optWriter.finalize();
    finalizeOptimizationStatistics(currentProblem, state);

    return true;
}

void Economy::incrementProgression(Progression::Task& progression)
{
    for (uint w = 0; w < pNbWeeks; ++w)
    {
        ++progression;
    }
}

// Retrieve weighted average balance for each area
static std::vector<AvgExchangeResults*> retrieveBalance(
  const Data::Study& study,
  Solver::Variable::Economy::AllVariables& variables)
{
    const uint nbAreas = study.areas.size();
    std::vector<AvgExchangeResults*> balance(nbAreas, nullptr);
    for (uint areaIndex = 0; areaIndex < nbAreas; ++areaIndex)
    {
        const Data::Area* area = study.areas.byIndex[areaIndex];
        variables.retrieveResultsForArea<Variable::Economy::VCardBalance>(&balance[areaIndex],
                                                                          area);
    }
    return balance;
}

void Economy::simulationEnd()
{
    if (!preproOnly && study.runtime.interconnectionsCount() > 0)
    {
        auto balance = retrieveBalance(study, variables);
        ComputeFlowQuad(study, pProblemesHebdo[0], balance, pNbWeeks);
    }
}

} // namespace Antares::Solver::Simulation
