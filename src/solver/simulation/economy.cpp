/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/

#include "economy.h"
#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/exception/AssertionError.hpp>
#include "simulation.h"
#include "../optimisation/opt_fonctions.h"
#include "../optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "common-eco-adq.h"

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Simulation
{
Economy::Economy(Data::Study& study, IResultWriter& resultWriter) :
    study(study),
    preproOnly(false),
    resultWriter(resultWriter)
{
}

Benchmarking::OptimizationInfo Economy::getOptimizationInfo() const
{
    const uint numSpace = 0;
    const auto& Pb = pProblemesHebdo[numSpace].ProblemeAResoudre;
    Benchmarking::OptimizationInfo optInfo;

    optInfo.nbVariables = Pb->NombreDeVariables;
    optInfo.nbConstraints = Pb->NombreDeContraintes;
    optInfo.nbNonZeroCoeffs = Pb->NombreDeTermesAllouesDansLaMatriceDesContraintes;
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
        weeklyOptProblems_.resize(pNbMaxPerformedYearsInParallel);
        postProcessesList_.resize(pNbMaxPerformedYearsInParallel);

        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            SIM_InitialisationProblemeHebdo(study, pProblemesHebdo[numSpace], 168, numSpace);

            if ((uint)nbHoursInAWeek != (uint)pProblemesHebdo[numSpace].NombreDePasDeTemps)
            {
                logs.fatal() << "internal error";
                return false;
            }

            auto options = createOptimizationOptions(study);
            weeklyOptProblems_[numSpace] =
                Antares::Solver::Optimization::WeeklyOptimization::create(
                                                    study,
                                                    options,
                                                    study.parameters.adqPatchParams,
                                                    &pProblemesHebdo[numSpace],
                                                    numSpace,
                                                    resultWriter);
            postProcessesList_[numSpace] =
                interfacePostProcessList::create(study.parameters.adqPatchParams,
                                                 &pProblemesHebdo[numSpace],
                                                 numSpace,
                                                 study.areas,
                                                 study.parameters.shedding.policy,
                                                 study.parameters.simplexOptimizationRange,
                                                 study.calendar);
        }
    }

    for (auto& pb : pProblemesHebdo)
        pb.TypeDOptimisation = OPTIMISATION_LINEAIRE;

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
                   OptimizationStatisticsWriter& optWriter)
{
    // No failed week at year start
    failedWeekList.clear();
    pProblemesHebdo[numSpace].year = state.year;

    PrepareRandomNumbers(study, pProblemesHebdo[numSpace], randomForYear);

    state.startANewYear();

    int hourInTheYear = pStartTime;
    if (isFirstPerformedYearOfSimulation)
        pProblemesHebdo[numSpace].firstWeekOfSimulation = true;
    bool reinitOptim = true;

    for (uint w = 0; w != pNbWeeks; ++w)
    {
        state.hourInTheYear = hourInTheYear;
        pProblemesHebdo[numSpace].weekInTheYear = state.weekInTheYear = w;
        pProblemesHebdo[numSpace].HeureDansLAnnee = hourInTheYear;

        ::SIM_RenseignementProblemeHebdo(study, pProblemesHebdo[numSpace], state.weekInTheYear,
                                         numSpace, hourInTheYear, hydroVentilationResults);

        BuildThermalPartOfWeeklyProblem(study, pProblemesHebdo[numSpace],
                                        hourInTheYear, randomForYear.pThermalNoisesByArea, state.year);

        // Reinit optimisation if needed
        pProblemesHebdo[numSpace].ReinitOptimisation = reinitOptim;
        reinitOptim = false;

        try
        {
            weeklyOptProblems_[numSpace]->solve();

            // Runs all the post processes in the list of post-process commands
            optRuntimeData opt_runtime_data(state.year, w, hourInTheYear);
            postProcessesList_[numSpace]->runAll(opt_runtime_data);

            variables.weekBegin(state);
            uint previousHourInTheYear = state.hourInTheYear;

            for (uint hw = 0; hw != nbHoursInAWeek;
                 ++hw, ++state.hourInTheYear, ++state.hourInTheSimulation)
            {
                state.hourInTheWeek = hw;

                state.ntc = pProblemesHebdo[numSpace].ValeursDeNTC[hw];

                variables.hourBegin(state.hourInTheYear);

                variables.hourForEachArea(state, numSpace);

                variables.hourEnd(state, state.hourInTheYear);
            }

            state.hourInTheYear = previousHourInTheYear;
            variables.weekForEachArea(state, numSpace);
            variables.weekEnd(state);

            for (int opt = 0; opt < 7; opt++)
            {
                state.optimalSolutionCost1 += pProblemesHebdo[numSpace].coutOptimalSolution1[opt];
                state.optimalSolutionCost2 += pProblemesHebdo[numSpace].coutOptimalSolution2[opt];
            }
            optWriter.addTime(w,
                              pProblemesHebdo[numSpace].tempsResolution1[0],
                              pProblemesHebdo[numSpace].tempsResolution2[0]);
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

        pProblemesHebdo[numSpace].firstWeekOfSimulation = false;

        ++progression;
    }

    updatingAnnualFinalHydroLevel(study.areas, pProblemesHebdo[numSpace]);

    optWriter.finalize();
    finalizeOptimizationStatistics(pProblemesHebdo[numSpace], state);

    return true;
}

void Economy::incrementProgression(Progression::Task& progression)
{
    for (uint w = 0; w < pNbWeeks; ++w)
        ++progression;
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
    if (!preproOnly && study.runtime->interconnectionsCount() > 0)
    {
        auto balance = retrieveBalance(study, variables);
        ComputeFlowQuad(study, pProblemesHebdo[0], balance, pNbWeeks);
    }
}

void Economy::prepareClustersInMustRunMode(uint numSpace, uint year)
{
    PrepareDataFromClustersInMustrunMode(study, numSpace, year);
}

} // namespace Antares::Solver::Simulation
