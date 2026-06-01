// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/economy.h"

#include <antares/exception/AssertionError.hpp>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/solver_utils.h"
#include "antares/writer/LegacySimulationTablesWriter.h"

using namespace Yuni;
using namespace Antares::Writer;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Simulation
{

Economy::Economy(Data::Study& study,
                 IResultWriter& resultWriter,
                 Simulation::ISimulationObserver& simulationObserver):
    study(study),
    preproOnly(false),
    resultWriter_(resultWriter),
    simulationObserver_(simulationObserver)
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

bool Economy::simulationBegin()
{
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
            if (study.parameters.include.reserves)
            {
                study.runtime.initializeReservesIndexMaps(study, pProblemesHebdo[numSpace]);
            }

            weeklyOptProblems_.emplace_back(study.parameters.optOptions,
                                            &pProblemesHebdo[numSpace],
                                            resultWriter_,
                                            simulationObserver_.get(),
                                            !study.parameters.noOutput);

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

bool Economy::year(Variable::State& state,
                   uint numSpace,
                   yearRandomNumbers& randomForYear,
                   std::list<uint>& failedWeekList,
                   const HYDRO_VENTILATION_RESULTS& hydroVentilationResults,
                   OptimizationStatisticsWriter& optWriter,
                   Benchmarking::DurationCollector& durationCollector,
                   const Antares::Data::Area::ScratchMap& scratchmap)
{
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
    auto* simulationTables = weeklyOptProblems_[numSpace].simulationTables();

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
        try
        {
            weeklyOptProblems_[numSpace].solve();

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
            // Indicate failed week list (first week of the year is "week number one" for the user
            // but w=0 for the loop)
            failedWeekList.push_back(w + 1);

            // Stop simulation
            logs.error("Assertion error for week " + std::to_string(w + 1)
                       + " simulation is stopped : " + ex.what());
            return false;
        }
        catch (Data::UnfeasibleProblemError&)
        {
            // need to clean next problemeHebdo

            // Indicate failed week list (first week of the year is "week number one" for the user
            // but w=0 for the loop)
            failedWeekList.push_back(w + 1);

            // Define if simulation must be stopped
            if (Data::stopSimulation(study.parameters.include.unfeasibleProblemBehavior))
            {
                return false;
            }
        }

        hourInTheYear += nbHoursInAWeek;
    }

    if (simulationTables && !study.folderOutput.empty())
    {
        LegacySimulationTablesWriter legacyWriter(study.folderOutput, state.year);
        legacyWriter.write(*simulationTables);
        simulationTables->clear();
    }

    optWriter.finalize();
    finalizeOptimizationStatistics(currentProblem, state);

    return true;
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
