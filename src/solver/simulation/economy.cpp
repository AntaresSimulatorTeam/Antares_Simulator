/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/simulation/economy.h"

#include <antares/exception/AssertionError.hpp>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/simulation.h"

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Simulation
{
Economy::Economy(Data::Study& study,
                 IResultWriter& resultWriter,
                 Simulation::ISimulationObserver& simulationObserver):
    study(study),
    preproOnly(false),
    resultWriter(resultWriter),
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

            auto options = createOptimizationOptions(study);

            weeklyOptProblems_.emplace_back(options,
                                            &pProblemesHebdo[numSpace],
                                            resultWriter,
                                            simulationObserver_.get());

            postProcessesList_[numSpace] = interfacePostProcessList::create(
              study.parameters.adqPatchParams,
              &pProblemesHebdo[numSpace],
              numSpace,
              study.areas,
              study.parameters.shedding.policy,
              study.parameters.simplexOptimizationRange,
              study.calendar);
        }
    }

    for (auto& pb: pProblemesHebdo)
    {
        pb.TypeDOptimisation = OPTIMISATION_LINEAIRE;
    }

    pStartTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    pNbWeeks = study.parameters.simulationDays.numberOfWeeks();
    return true;
}

bool Economy::year(Progression::Task& progression,
                   Variable::State& state,
                   uint numSpace,
                   yearRandomNumbers& randomForYear,
                   std::list<uint>& failedWeekList,
                   bool isFirstPerformedYearOfSimulation,
                   const HYDRO_VENTILATION_RESULTS& hydroVentilationResults,
                   OptimizationStatisticsWriter& optWriter,
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
    if (isFirstPerformedYearOfSimulation)
    {
        currentProblem.firstWeekOfSimulation = true;
    }
    bool reinitOptim = true;

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

        // Reinit optimisation if needed
        currentProblem.ReinitOptimisation = reinitOptim;
        reinitOptim = false;

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
            reinitOptim = true;

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

        currentProblem.firstWeekOfSimulation = false;

        ++progression;
    }

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

void Economy::prepareClustersInMustRunMode(Data::Area::ScratchMap& scratchmap, uint year)
{
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *study.areas[i];
        auto& scratchpad = scratchmap.at(&area);

        std::ranges::fill(scratchpad.mustrunSum, 0);

        auto& mrs = scratchpad.mustrunSum;
        for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
        {
            const auto& availableProduction = cluster->series.getColumn(year);
            for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
            {
                mrs[h] += availableProduction[h];
            }
        }
    }
}

} // namespace Antares::Solver::Simulation
