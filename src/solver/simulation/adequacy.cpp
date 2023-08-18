/*
** Copyright 2007-2023 RTE
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

#include "adequacy.h"
#include <antares/exception/UnfeasibleProblemError.hpp>
#include <antares/exception/AssertionError.hpp>
#include "opt_time_writer.h"

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Simulation
{
Adequacy::Adequacy(Data::Study& study) : study(study), preproOnly(false)
{
}

Benchmarking::OptimizationInfo Adequacy::getOptimizationInfo() const
{
    const uint numSpace = 0;
    const auto& Pb = pProblemesHebdo[numSpace].ProblemeAResoudre;
    Benchmarking::OptimizationInfo optInfo;

    optInfo.nbVariables = Pb->NombreDeVariables;
    optInfo.nbConstraints = Pb->NombreDeContraintes;
    optInfo.nbNonZeroCoeffs = Pb->NombreDeTermesAllouesDansLaMatriceDesContraintes;
    return optInfo;
}

void Adequacy::setNbPerformedYearsInParallel(uint nbMaxPerformedYearsInParallel)
{
    pNbMaxPerformedYearsInParallel = nbMaxPerformedYearsInParallel;
}

void Adequacy::initializeState(Variable::State& state, uint numSpace)
{
    state.problemeHebdo = &pProblemesHebdo[numSpace];
    state.resSpilled.reset(study.areas.size(), (uint)nbHoursInAWeek);
    state.numSpace = numSpace;
}

bool Adequacy::simulationBegin()
{
    if (!preproOnly)
    {
        pProblemesHebdo.resize(pNbMaxPerformedYearsInParallel);
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            SIM_InitialisationProblemeHebdo(study, pProblemesHebdo[numSpace], 168, numSpace);

            assert((uint)nbHoursInAWeek == (uint)pProblemesHebdo[numSpace].NombreDePasDeTemps
                   && "inconsistency");
            if ((uint)nbHoursInAWeek != (uint)pProblemesHebdo[numSpace].NombreDePasDeTemps)
            {
                logs.fatal() << "internal error";
                return false;
            }
        }
    }

    if (!pProblemesHebdo.empty())
    {
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
            pProblemesHebdo[numSpace].TypeDOptimisation = OPTIMISATION_LINEAIRE;
    }

    pStartTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    pNbWeeks = study.parameters.simulationDays.numberOfWeeks();
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
            auto& valgen = ValeursGenereesParPays[numSpace][k];

            double quantity
              = pProblemesHebdo[numSpace].ConsommationsAbattues[j].ConsommationAbattueDuPays[k]
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
                    std::list<uint>& failedWeekList,
                    bool isFirstPerformedYearOfSimulation)
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

    OptimizationStatisticsWriter optWriter(study.resultWriter, state.year);

    for (uint w = 0; w != pNbWeeks; ++w)
    {
        state.hourInTheYear = hourInTheYear;
        pProblemesHebdo[numSpace].weekInTheYear = state.weekInTheYear = w;
        pProblemesHebdo[numSpace].HeureDansLAnnee = hourInTheYear;

        ::SIM_RenseignementProblemeHebdo(study,
          pProblemesHebdo[numSpace], state.weekInTheYear, numSpace, hourInTheYear);

        // Reinit optimisation if needed
        pProblemesHebdo[numSpace].ReinitOptimisation = reinitOptim;
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
                                      .ConsommationsAbattues[hw]
                                      .ConsommationAbattueDuPays[ar];
                    double stratReserve
                      = area.reserves[Data::fhrStrategicReserve][hw + hourInTheYear];
                    assert(ar < state.resSpilled.width);
                    assert(hw < state.resSpilled.height);

                    if (conso < -stratReserve)
                    {
                        conso += stratReserve;
                        state.resSpilled[ar][hw] = stratReserve;
                    }
                    else
                    {
                        if (conso < 0.)
                        {
                            state.resSpilled[ar][hw] = -conso;
                            conso = 0.;
                        }
                        else
                            state.resSpilled[ar][hw] = 0.;
                    }
                }
            }

            try
            {
                OPT_OptimisationHebdomadaire(createOptimizationOptions(study),
                                             &pProblemesHebdo[numSpace],
                                             study.parameters.adqPatchParams,
                                             *study.resultWriter);

                computingHydroLevels(study.areas, pProblemesHebdo[numSpace], false);

                RemixHydroForAllAreas(study.areas,
                                      pProblemesHebdo[numSpace],
                                      study.parameters.shedding.policy,
                                      study.parameters.simplexOptimizationRange,
                                      numSpace,
                                      hourInTheYear);

                computingHydroLevels(study.areas, pProblemesHebdo[numSpace], true);
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
            catch (Data::UnfeasibleProblemError&)
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
                  = pProblemesHebdo[numSpace].VariablesDualesDesContraintesDeNTC[i];
                for (uint lnkindex = 0; lnkindex != runtime.interconnectionsCount(); ++lnkindex)
                    varduales.VariableDualeParInterconnexion[lnkindex] = 0.;
            }

            for (uint hw = 0; hw != nbHoursInAWeek; ++hw)
            {
                auto& ntc = pProblemesHebdo[numSpace].ValeursDeNTC[hw];
                ntc.ValeurDuFlux.resize(runtime.interconnectionsCount(), 0);
            }

            for (uint ar = 0; ar != nbAreas; ++ar)
            {
                auto& hourlyResults = pProblemesHebdo[numSpace].ResultatsHoraires[ar];

                std::fill(hourlyResults.ValeursHorairesDeDefaillancePositive.begin(),
                        hourlyResults.ValeursHorairesDeDefaillancePositive.end(), 0);

                std::fill(hourlyResults.ValeursHorairesDeDefaillanceNegative.begin(),
                        hourlyResults.ValeursHorairesDeDefaillanceNegative.end(), 0);

                std::fill(hourlyResults.CoutsMarginauxHoraires.begin(),
                        hourlyResults.CoutsMarginauxHoraires.end(), 0);

                std::fill(hourlyResults.PompageHoraire.begin(),
                        hourlyResults.PompageHoraire.end(), 0);

                std::fill(hourlyResults.debordementsHoraires.begin(),
                        hourlyResults.debordementsHoraires.end(), 0);

                std::fill(hourlyResults.niveauxHoraires.begin(),
                        hourlyResults.niveauxHoraires.end(), 0);
            }

            uint indx = hourInTheYear;

            for (uint j = 0; j != nbHoursInAWeek; ++j, ++indx)
            {
                uint dayInTheYear = study.calendar.hours[indx].dayYear;

                for (uint k = 0; k != nbAreas; ++k)
                {
                    assert(k < state.resSpilled.width);
                    assert(j < state.resSpilled.height);
                    auto& valgen = ValeursGenereesParPays[numSpace][k];
                    auto& hourlyResults = pProblemesHebdo[numSpace].ResultatsHoraires[k];

                    hourlyResults.TurbinageHoraire[j]
                      = valgen.HydrauliqueModulableQuotidien[dayInTheYear] / 24.;

                    state.resSpilled[k][j]
                      = +valgen.HydrauliqueModulableQuotidien[dayInTheYear] / 24.
                        - pProblemesHebdo[numSpace]
                            .ConsommationsAbattues[j]
                            .ConsommationAbattueDuPays[k];
                }
            }

            computingHydroLevels(study.areas, pProblemesHebdo[numSpace], false, true);
        }

        interpolateWaterValue(
          study.areas, pProblemesHebdo[numSpace], study.calendar, hourInTheYear);

        updatingWeeklyFinalHydroLevel(study.areas, pProblemesHebdo[numSpace]);

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

        hourInTheYear += nbHoursInAWeek;

        pProblemesHebdo[numSpace].firstWeekOfSimulation = false;

        optWriter.addTime(w,
                          pProblemesHebdo[numSpace].tempsResolution1[0],
                          pProblemesHebdo[numSpace].tempsResolution2[0]);

        ++progression;
    }

    updatingAnnualFinalHydroLevel(study.areas, pProblemesHebdo[numSpace]);

    optWriter.finalize();
    finalizeOptimizationStatistics(pProblemesHebdo[numSpace], state);

    return true;
}

void Adequacy::incrementProgression(Progression::Task& progression)
{
    for (uint w = 0; w < pNbWeeks; ++w)
        ++progression;
}

// Retrieve weighted average balance for each area
static std::vector<AvgExchangeResults*> retrieveBalance(
  const Data::Study& study,
  Solver::Variable::Adequacy::AllVariables& variables)
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

void Adequacy::simulationEnd()
{
    if (!preproOnly && study.runtime->interconnectionsCount() > 0)
    {
        auto balance = retrieveBalance(study, variables);
        ComputeFlowQuad(study, pProblemesHebdo[0], balance, pNbWeeks);
    }
}

void Adequacy::prepareClustersInMustRunMode(uint numSpace)
{
    PrepareDataFromClustersInMustrunMode(study, numSpace);
}

} // namespace Antares::Solver::Simulation
