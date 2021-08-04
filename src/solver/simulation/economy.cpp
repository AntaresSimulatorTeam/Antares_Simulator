/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <yuni/yuni.h>
#include <antares/study/memory-usage.h>
#include "economy.h"
#include <antares/study.h>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/exception/AssertionError.hpp>
#include <yuni/core/math.h>
#include "simulation.h"
#include "../optimisation/opt_fonctions.h"
#include "common-eco-adq.h"

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace Simulation
{
enum
{

    nbHoursInAWeek = 168,
};

Economy::Economy(Data::Study& study) : study(study), preproOnly(false), pProblemesHebdo(nullptr)
{
}

Economy::~Economy()
{
    if (pProblemesHebdo)
    {
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            OPT_LiberationMemoireDuProblemeAOptimiser(pProblemesHebdo[numSpace]);
            SIM_DesallocationProblemeHebdo(*pProblemesHebdo[numSpace]);
            delete pProblemesHebdo[numSpace];
        }
        delete[] pProblemesHebdo;
    }
}

void Economy::setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel)
{
    pNbMaxPerformedYearsInParallel = nbMaxPerformedYearsInParallel;
}

void Economy::initializeState(Variable::State& state, uint numSpace)
{
    state.problemeHebdo = pProblemesHebdo[numSpace];
}

bool Economy::simulationBegin()
{
    if (!preproOnly)
    {
        pProblemesHebdo = new PROBLEME_HEBDO*[pNbMaxPerformedYearsInParallel];
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            pProblemesHebdo[numSpace] = new PROBLEME_HEBDO();
            memset(pProblemesHebdo[numSpace], '\0', sizeof(PROBLEME_HEBDO));
            SIM_InitialisationProblemeHebdo(study, *pProblemesHebdo[numSpace], 168, numSpace);

            assert((uint)nbHoursInAWeek == (uint)pProblemesHebdo[numSpace]->NombreDePasDeTemps
                   && "inconsistency");
            if ((uint)nbHoursInAWeek != (uint)pProblemesHebdo[numSpace]->NombreDePasDeTemps)
            {
                logs.fatal() << "internal error";
                return false;
            }
        }

        SIM_InitialisationResultats();
    }

    if (pProblemesHebdo)
    {
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
            pProblemesHebdo[numSpace]->TypeDOptimisation = OPTIMISATION_LINEAIRE;
    }

    pStartTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    pNbWeeks = (study.parameters.simulationDays.end - study.parameters.simulationDays.first) / 7;
    return true;
}

bool Economy::year(Progression::Task& progression,
                   Variable::State& state,
                   uint numSpace,
                   yearRandomNumbers& randomForYear,
                   std::list<uint>& failedWeekList,
                   bool isFirstPerformedYearOfSimulation)
{
    // No failed week at year start
    failedWeekList.clear();

    PrepareRandomNumbers(study, *pProblemesHebdo[numSpace], randomForYear);

    state.startANewYear();

    int hourInTheYear = pStartTime;
    if (isFirstPerformedYearOfSimulation)
        pProblemesHebdo[numSpace]->firstWeekOfSimulation = true;
    bool reinitOptim = true;

    for (uint w = 0; w != pNbWeeks; ++w)
    {
        state.hourInTheYear = hourInTheYear;
        state.study.runtime->weekInTheYear[numSpace] = state.weekInTheYear = w;
        pProblemesHebdo[numSpace]->HeureDansLAnnee = hourInTheYear;

        ::SIM_RenseignementProblemeHebdo(
          *pProblemesHebdo[numSpace], state, numSpace, hourInTheYear);

        // Reinit optimisation if needed
        pProblemesHebdo[numSpace]->ReinitOptimisation = reinitOptim ? OUI_ANTARES : NON_ANTARES;
        reinitOptim = false;

        try
        {
            OPT_OptimisationHebdomadaire(pProblemesHebdo[numSpace], numSpace);
            DispatchableMarginForAllAreas(
              study, *pProblemesHebdo[numSpace], numSpace, hourInTheYear, nbHoursInAWeek);

            computingHydroLevels(study, *pProblemesHebdo[numSpace], nbHoursInAWeek, false);

            RemixHydroForAllAreas(
              study, *pProblemesHebdo[numSpace], numSpace, hourInTheYear, nbHoursInAWeek);

            computingHydroLevels(study, *pProblemesHebdo[numSpace], nbHoursInAWeek, true);

            interpolateWaterValue(
              study, *pProblemesHebdo[numSpace], state, hourInTheYear, nbHoursInAWeek);

            updatingWeeklyFinalHydroLevel(study, *pProblemesHebdo[numSpace], nbHoursInAWeek);

            variables.weekBegin(state);
            uint previousHourInTheYear = state.hourInTheYear;

            for (uint hw = 0; hw != nbHoursInAWeek;
                 ++hw, ++state.hourInTheYear, ++state.hourInTheSimulation)
            {
                state.hourInTheWeek = hw;

                state.ntc = pProblemesHebdo[numSpace]->ValeursDeNTC[hw];

                variables.hourBegin(state.hourInTheYear);

                variables.hourForEachArea(state, numSpace);

                variables.hourEnd(state, state.hourInTheYear);
            }

            state.hourInTheYear = previousHourInTheYear;
            variables.weekForEachArea(state, numSpace);
            variables.weekEnd(state);

            for (int opt = 0; opt < 7; opt++)
            {
                state.optimalSolutionCost1 += pProblemesHebdo[numSpace]->coutOptimalSolution1[opt];
                state.optimalSolutionCost2 += pProblemesHebdo[numSpace]->coutOptimalSolution2[opt];
            }
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

        pProblemesHebdo[numSpace]->firstWeekOfSimulation = false;

        ++progression;
    }

    updatingAnnualFinalHydroLevel(study, *pProblemesHebdo[numSpace]);

    logs.info() << "Resolution statistics";
    logs.info() << " Total running time: " << std::to_string(pProblemesHebdo[numSpace]->optimizationStatistics_object.getTotalSolvingTime()) << "ms";
    logs.info() << " Number of resolution: " << std::to_string(pProblemesHebdo[numSpace]->optimizationStatistics_object.getNbSolve());
    logs.info() << " Average resolution time: " << std::to_string(pProblemesHebdo[numSpace]->optimizationStatistics_object.getAverageSolvingTime()) << "ms";

    return true;
}

void Economy::incrementProgression(Progression::Task& progression)
{
    for (uint w = 0; w < pNbWeeks; ++w)
        ++progression;
}

AvgExchangeResults* Economy::callbackRetrieveBalanceData(Data::Area* area)
{
    AvgExchangeResults* balance = nullptr;
    variables.retrieveResultsForArea<Variable::Economy::VCardBalance>(&balance, area);
    return balance;
}

void Economy::simulationEnd()
{
    if (!preproOnly && study.runtime->interconnectionsCount > 0)
    {
        CallbackBalanceRetrieval callback;
        callback.bind(this, &Economy::callbackRetrieveBalanceData);
        PerformQuadraticOptimisation(study, *pProblemesHebdo[0], callback, pNbWeeks);
    }
}

void Economy::prepareClustersInMustRunMode(uint numSpace)
{
    PrepareDataFromClustersInMustrunMode(study, numSpace);
}

} // namespace Simulation
} // namespace Solver
} // namespace Antares
