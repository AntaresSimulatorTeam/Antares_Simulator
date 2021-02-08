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
#include "adequacy.h"
#include <antares/study.h>
#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/exception/AssertionError.hpp>
#include <yuni/core/math.h>
#include "simulation.h"
#include "../optimisation/opt_fonctions.h"
#include "common-eco-adq.h"
#include "sim_structure_probleme_economique.h"

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

Adequacy::Adequacy(Data::Study& study) : study(study), preproOnly(false), pProblemesHebdo(nullptr)
{
}

Adequacy::~Adequacy()
{
    if (pProblemesHebdo)
    {
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            OPT_LiberationMemoireDuProblemeAOptimiser(pProblemesHebdo[numSpace]);
            SIM_DesallocationProblemeHebdo(*pProblemesHebdo[numSpace]);
            delete pProblemesHebdo[numSpace];
        }
        delete pProblemesHebdo;
    }
}

void Adequacy::setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel)
{
    pNbMaxPerformedYearsInParallel = nbMaxPerformedYearsInParallel;
}

void Adequacy::initializeState(Variable::State& state, uint numSpace)
{
    state.problemeHebdo = pProblemesHebdo[numSpace];
    state.resSpilled.reset(study.areas.size(), (uint)nbHoursInAWeek);
}

bool Adequacy::simulationBegin()
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

bool Adequacy::simplexIsRequired(uint hourInTheYear, uint numSpace) const
{
    uint areaCount = study.areas.size();
    uint indx = hourInTheYear;

    for (uint j = 0; j != nbHoursInAWeek; ++j, ++indx)
    {
        uint dayInTheYear = study.calendar.hours[indx].dayYear;

        for (uint k = 0; k != areaCount; ++k)
        {
            auto& valgen = *ValeursGenereesParPays[numSpace][k];

            double quantity
              = pProblemesHebdo[numSpace]->ConsommationsAbattues[j]->ConsommationAbattueDuPays[k]
                - valgen.HydrauliqueModulableQuotidien[dayInTheYear] / 24.;

            if (quantity > 0.)
                return true; // Call to the solver is required to find an optimal solution
        }
    }

    return false; // No need to call the solver to exhibit an optimal solution
}

bool Adequacy::year(Progression::Task& progression,
                    Variable::State& state,
                    uint numSpace,
                    yearRandomNumbers& randomForYear,
                    std::list<uint>& failedWeekList)
{
    // No failed week at year start
    failedWeekList.clear();

    PrepareRandomNumbers(study, *pProblemesHebdo[numSpace], randomForYear);

    state.startANewYear();

    int hourInTheYear = pStartTime;
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

        state.simplexHasBeenRan = (w == 0) || simplexIsRequired(hourInTheYear, numSpace);
        if (state.simplexHasBeenRan) // Call to Solver is mandatory for the first week and optional
                                     // otherwise
        {
            uint nbAreas = study.areas.size();
            for (uint ar = 0; ar != nbAreas; ++ar)
            {
                auto& area = *(study.areas.byIndex[ar]);
                for (uint hw = 0; hw != nbHoursInAWeek; ++hw)
                {
                    double& conso = pProblemesHebdo[numSpace]
                                      ->ConsommationsAbattues[hw]
                                      ->ConsommationAbattueDuPays[ar];
                    double& conso2 = pProblemesHebdo[numSpace]
                                       ->ConsommationsAbattuesRef[hw]
                                       ->ConsommationAbattueDuPays[ar];
                    double stratReserve
                      = area.reserves[Data::fhrStrategicReserve][hw + hourInTheYear];
                    assert(ar < state.resSpilled.width);
                    assert(hw < state.resSpilled.height);

                    if (conso < -stratReserve)
                    {
                        conso += stratReserve;
                        conso2 += stratReserve;
                        state.resSpilled[ar][hw] = stratReserve;
                    }
                    else
                    {
                        if (conso < 0.)
                        {
                            state.resSpilled[ar][hw] = -conso;
                            conso = 0.;
                            conso2 = 0.;
                        }
                        else
                            state.resSpilled[ar][hw] = 0.;
                    }
                }

                area.reserves.flush();
            }

            try
            {
                OPT_OptimisationHebdomadaire(pProblemesHebdo[numSpace], numSpace);

                computingHydroLevels(study, *pProblemesHebdo[numSpace], nbHoursInAWeek, false);

                RemixHydroForAllAreas(
                  study, *pProblemesHebdo[numSpace], numSpace, hourInTheYear, nbHoursInAWeek);

                computingHydroLevels(study, *pProblemesHebdo[numSpace], nbHoursInAWeek, true);
            }
            catch (Data::AssertionError& ex)
            {
                // Indicate failed week list (first week of the year is "week number one" for the
                // user but w=0 for the loop)
                failedWeekList.push_back(w + 1);

                // Stop simulation
                logs.error("Assertion error for week " + std::to_string(w + 1)
                           + " simulation is stopped : " + ex.what());
                return false;
            }
            catch (Data::UnfeasibleProblemError& ex)
            {
                // need to clean next problemeHebdo
                reinitOptim = true;

                // Indicate failed week list (first week of the year is "week number one" for the
                // user but w=0 for the loop)
                failedWeekList.push_back(w + 1);

                // Define if simulation must be stopped
                if (Data::stopSimulation(study.parameters.include.unfeasibleProblemBehavior))
                {
                    return false;
                }
            }
        }
        else
        {
            state.resSpilled.zero();

            auto nbAreas = study.areas.size();
            auto& runtime = *(study.runtime);

            for (uint i = 0; i != nbHoursInAWeek; ++i)
            {
                auto& varduales
                  = *(pProblemesHebdo[numSpace]->VariablesDualesDesContraintesDeNTC[i]);
                for (uint lnkindex = 0; lnkindex != runtime.interconnectionsCount; ++lnkindex)
                    varduales.VariableDualeParInterconnexion[lnkindex] = 0.;
            }

            for (uint hw = 0; hw != nbHoursInAWeek; ++hw)
            {
                auto* ntc = pProblemesHebdo[numSpace]->ValeursDeNTC[hw];
                memset(ntc->ValeurDuFlux, 0, sizeof(double) * runtime.interconnectionsCount);
            }

            for (uint ar = 0; ar != nbAreas; ++ar)
            {
                auto& hourlyResults = *(pProblemesHebdo[numSpace]->ResultatsHoraires[ar]);

                memset(hourlyResults.ValeursHorairesDeDefaillancePositive,
                       0,
                       sizeof(double) * nbHoursInAWeek);
                memset(hourlyResults.ValeursHorairesDeDefaillanceNegative,
                       0,
                       sizeof(double) * nbHoursInAWeek);
                memset(hourlyResults.CoutsMarginauxHoraires, 0, sizeof(double) * nbHoursInAWeek);
                memset(hourlyResults.PompageHoraire, 0, sizeof(double) * nbHoursInAWeek);
                memset(hourlyResults.debordementsHoraires, 0, sizeof(double) * nbHoursInAWeek);
                memset(hourlyResults.niveauxHoraires, 0, sizeof(double) * nbHoursInAWeek);
            }

            uint indx = hourInTheYear;

            for (uint j = 0; j != nbHoursInAWeek; ++j, ++indx)
            {
                uint dayInTheYear = study.calendar.hours[indx].dayYear;

                for (uint k = 0; k != nbAreas; ++k)
                {
                    assert(k < state.resSpilled.width);
                    assert(j < state.resSpilled.height);
                    auto& valgen = *ValeursGenereesParPays[numSpace][k];
                    auto& hourlyResults = *(pProblemesHebdo[numSpace]->ResultatsHoraires[k]);

                    hourlyResults.TurbinageHoraire[j]
                      = valgen.HydrauliqueModulableQuotidien[dayInTheYear] / 24.;

                    state.resSpilled[k][j]
                      = +valgen.HydrauliqueModulableQuotidien[dayInTheYear] / 24.
                        - pProblemesHebdo[numSpace]
                            ->ConsommationsAbattues[j]
                            ->ConsommationAbattueDuPays[k];
                }
            }

            computingHydroLevels(study, *pProblemesHebdo[numSpace], nbHoursInAWeek, false, true);
        }

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

        hourInTheYear += nbHoursInAWeek;

        ++progression;
    }

    updatingAnnualFinalHydroLevel(study, *pProblemesHebdo[numSpace]);

    return true;
}

void Adequacy::incrementProgression(Progression::Task& progression)
{
    for (uint w = 0; w < pNbWeeks; ++w)
        ++progression;
}

AvgExchangeResults* Adequacy::callbackRetrieveBalanceData(Data::Area* area)
{
    AvgExchangeResults* balance = nullptr;
    variables.retrieveResultsForArea<Variable::Economy::VCardBalance>(&balance, area);
    return balance;
}

void Adequacy::simulationEnd()
{
    if (!preproOnly && study.runtime->interconnectionsCount > 0)
    {
        CallbackBalanceRetrieval callback;
        callback.bind(this, &Adequacy::callbackRetrieveBalanceData);
        PerformQuadraticOptimisation(study, *pProblemesHebdo[0], callback, pNbWeeks);
    }
}

void Adequacy::prepareClustersInMustRunMode(uint numSpace)
{
    PrepareDataFromClustersInMustrunMode(study, numSpace);
}

} // namespace Simulation
} // namespace Solver
} // namespace Antares
