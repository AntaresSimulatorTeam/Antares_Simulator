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

using namespace Yuni;
using Antares::Constants::nbHoursInAWeek;

namespace Antares::Solver::Simulation
{
Adequacy::Adequacy(Data::Study& study, IResultWriter& resultWriter) :
    study(study),
    preproOnly(false),
    resultWriter(resultWriter)
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

// valGen maybe_unused to match simulationBegin() declaration in economy.cpp
bool Adequacy::simulationBegin()
{
    if (!preproOnly)
    {
        pProblemesHebdo.resize(pNbMaxPerformedYearsInParallel);
        for (uint numSpace = 0; numSpace < pNbMaxPerformedYearsInParallel; numSpace++)
        {
            Antares::Data::Area::ScratchMap scratchmap;
            study.areas.each([&scratchmap, &numSpace](Antares::Data::Area& a) {
                    scratchmap.try_emplace(&a, a.scratchpad[numSpace]); });

            SIM_InitialisationProblemeHebdo(study, pProblemesHebdo[numSpace], 168, scratchmap);

            assert((uint)nbHoursInAWeek == (uint)pProblemesHebdo[numSpace].NombreDePasDeTemps
                   && "inconsistency");
            if ((uint)nbHoursInAWeek != (uint)pProblemesHebdo[numSpace].NombreDePasDeTemps)
            {
                logs.fatal() << "internal error";
                return false;
            }
        }
    }

    for (auto& pb : pProblemesHebdo)
        pb.TypeDOptimisation = OPTIMISATION_LINEAIRE;

    pStartTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;
    pNbWeeks = study.parameters.simulationDays.numberOfWeeks();
    return true;
}

bool Adequacy::simplexIsRequired(uint hourInTheYear,
                                 uint numSpace,
                                 const HYDRO_VENTILATION_RESULTS& hydroVentilationResults) const
{
    uint areaCount = study.areas.size();
    uint indx = hourInTheYear;

    for (uint j = 0; j != nbHoursInAWeek; ++j, ++indx)
    {
        uint dayInTheYear = study.calendar.hours[indx].dayYear;

        for (uint areaIdx = 0; areaIdx != areaCount; ++areaIdx)
        {
            auto& hydroVentilation = hydroVentilationResults[areaIdx];

            double quantity
              = pProblemesHebdo[numSpace].ConsommationsAbattues[j].ConsommationAbattueDuPays[areaIdx]
                - hydroVentilation.HydrauliqueModulableQuotidien[dayInTheYear] / 24.;

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

    state.startANewYear();

    int hourInTheYear = pStartTime;
    if (isFirstPerformedYearOfSimulation)
        currentProblem.firstWeekOfSimulation = true;
    bool reinitOptim = true;

    for (uint w = 0; w != pNbWeeks; ++w)
    {
        state.hourInTheYear = hourInTheYear;
        currentProblem.weekInTheYear = state.weekInTheYear = w;
        currentProblem.HeureDansLAnnee = hourInTheYear;

        ::SIM_RenseignementProblemeHebdo(study, currentProblem, state.weekInTheYear,
                                         hourInTheYear, hydroVentilationResults, scratchmap);

        BuildThermalPartOfWeeklyProblem(study, currentProblem,
                                        hourInTheYear, randomForYear.pThermalNoisesByArea, state.year);

        // Reinit optimisation if needed
        currentProblem.ReinitOptimisation = reinitOptim;
        reinitOptim = false;

        state.simplexRunNeeded = (w == 0) || simplexIsRequired(hourInTheYear, numSpace, hydroVentilationResults);
        if (state.simplexRunNeeded) // Call to Solver is mandatory for the first week and optional
                                     // otherwise
        {
            uint nbAreas = study.areas.size();
            for (uint ar = 0; ar != nbAreas; ++ar)
            {
                auto& area = *(study.areas.byIndex[ar]);
                for (uint hw = 0; hw != nbHoursInAWeek; ++hw)
                {
                    double& conso = currentProblem
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
                                             &currentProblem,
                                             study.parameters.adqPatchParams,
                                             resultWriter);

                computingHydroLevels(study.areas, currentProblem, false);

                RemixHydroForAllAreas(study.areas,
                                      currentProblem,
                                      study.parameters.shedding.policy,
                                      study.parameters.simplexOptimizationRange,
                                      numSpace,
                                      hourInTheYear);

                computingHydroLevels(study.areas, currentProblem, true);
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
                  = currentProblem.VariablesDualesDesContraintesDeNTC[i];
                for (uint lnkindex = 0; lnkindex != runtime.interconnectionsCount(); ++lnkindex)
                    varduales.VariableDualeParInterconnexion[lnkindex] = 0.;
            }

            for (uint hw = 0; hw != nbHoursInAWeek; ++hw)
            {
                auto& ntc = currentProblem.ValeursDeNTC[hw];
                ntc.ValeurDuFlux.resize(runtime.interconnectionsCount(), 0);
            }

            for (uint ar = 0; ar != nbAreas; ++ar)
            {
                auto& hourlyResults = currentProblem.ResultatsHoraires[ar];

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
                    auto& hydroVentilation = hydroVentilationResults[k];
                    auto& hourlyResults = currentProblem.ResultatsHoraires[k];

                    hourlyResults.TurbinageHoraire[j]
                      = hydroVentilation.HydrauliqueModulableQuotidien[dayInTheYear] / 24.;

                    state.resSpilled[k][j]
                      = +hydroVentilation.HydrauliqueModulableQuotidien[dayInTheYear] / 24.
                        - currentProblem
                            .ConsommationsAbattues[j]
                            .ConsommationAbattueDuPays[k];
                }
            }

            computingHydroLevels(study.areas, currentProblem, false, true);
        }

        interpolateWaterValue(
          study.areas, currentProblem, study.calendar, hourInTheYear);

        updatingWeeklyFinalHydroLevel(study.areas, currentProblem);

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

        hourInTheYear += nbHoursInAWeek;

        currentProblem.firstWeekOfSimulation = false;

        optWriter.addTime(w,
                          currentProblem.tempsResolution1[0],
                          currentProblem.tempsResolution2[0]);

        ++progression;
    }

    updatingAnnualFinalHydroLevel(study.areas, currentProblem);

    optWriter.finalize();
    finalizeOptimizationStatistics(currentProblem, state);

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

void Adequacy::prepareClustersInMustRunMode(Data::Area::ScratchMap& scratchmap, uint year)
{
    PrepareDataFromClustersInMustrunMode(study, scratchmap, year);
}

} // namespace Antares::Solver::Simulation
