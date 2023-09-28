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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <antares/study/study.h>
#include <antares/study/area/scratchpad.h>
#include <antares/exception/UnfeasibleProblemError.hpp>

#include "common-eco-adq.h"
#include <antares/logs/logs.h>
#include <cassert>
#include <map>
#include "simulation.h"
#include "../aleatoire/alea_fonctions.h"

using namespace Yuni;

namespace Antares::Solver::Simulation
{
static void RecalculDesEchangesMoyens(Data::Study& study,
                                      PROBLEME_HEBDO& problem,
                                      const std::vector<AvgExchangeResults*>& balance,
                                      int PasDeTempsDebut)
{
    for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; i++)
    {
        auto& ntcValues = problem.ValeursDeNTC[i];
        uint decalPasDeTemps = PasDeTempsDebut + i;

        for (uint j = 0; j < study.areas.size(); ++j)
        {
            assert(balance[j] && "Impossible to find the variable");
            if (balance[j])
            {
                problem.SoldeMoyenHoraire[i].SoldeMoyenDuPays[j]
                  = balance[j]->avgdata.hourly[decalPasDeTemps];
            }
            else
            {
                assert(false && "invalid balance");
                problem.SoldeMoyenHoraire[i].SoldeMoyenDuPays[j] = 0.0;
            }
        }

        std::vector<double> avgDirect;
        std::vector<double> avgIndirect;
        for (uint j = 0; j < study.runtime->interconnectionsCount(); ++j)
        {
            auto* link = study.runtime->areaLink[j];
            int ret = retrieveAverageNTC(
              study, link->directCapacities, link->timeseriesNumbers, avgDirect);

            ret = retrieveAverageNTC(
                    study, link->indirectCapacities, link->timeseriesNumbers, avgIndirect)
                  && ret;
            if (!ret)
            {
                ntcValues.ValeurDeNTCOrigineVersExtremite[j] = avgDirect[decalPasDeTemps];
                ntcValues.ValeurDeNTCExtremiteVersOrigine[j] = avgIndirect[decalPasDeTemps];
            }
            else
            {
                assert(false && "invalid NTC");
            }

            auto& mtxParamaters = link->parameters;
            ntcValues.ResistanceApparente[j] = mtxParamaters[Data::fhlImpedances][decalPasDeTemps];
        }
    }

    try
    {
        OPT_OptimisationHebdomadaire(createOptimizationOptions(study), &problem, study.parameters.adqPatchParams, *study.resultWriter);
    }
    catch (Data::UnfeasibleProblemError&)
    {
        study.runtime->quadraticOptimizationHasFailed = true;
    }

    for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; ++i)
    {
        const uint indx = i + PasDeTempsDebut;
        auto& ntcValues = problem.ValeursDeNTC[i];
        assert(&ntcValues);

        for (uint j = 0; j < study.runtime->interconnectionsCount(); ++j)
        {
            transitMoyenInterconnexionsRecalculQuadratique[j][indx]
              = ntcValues.ValeurDuFlux[j];
        }
    }
}

void PrepareDataFromClustersInMustrunMode(Data::Study& study, uint numSpace, uint year)
{
    bool inAdequacy = (study.parameters.mode == Data::stdmAdequacy);

    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *study.areas[i];
        auto& scratchpad = area.scratchpad[numSpace];

        memset(scratchpad.mustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);
        if (inAdequacy)
            memset(scratchpad.originalMustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);

        double* mrs = scratchpad.mustrunSum;
        double* adq = scratchpad.originalMustrunSum;

        if (!area.thermal.mustrunList.empty())
        {
            auto end = area.thermal.mustrunList.end();
            for (auto i = area.thermal.mustrunList.begin(); i != end; ++i)
            {
                auto& cluster = *(i->second);

                if (inAdequacy && cluster.mustrunOrigin)
                {
                    for (uint h = 0; h != 168; ++h)
                    {
                        mrs[h] += cluster.series->getAvailablePower(h, year);
                        adq[h] += cluster.series->getAvailablePower(h, year);
                    }
                }
                else
                {
                    for (uint h = 0; h != 168; ++h)
                        mrs[h] += cluster.series->getAvailablePower(h, year);
                }
            }
        }

        if (inAdequacy)
        {
            auto end = area.thermal.list.end();
            for (auto i = area.thermal.list.begin(); i != end; ++i)
            {
                auto& cluster = *(i->second);
                if (!cluster.mustrunOrigin)
                    continue;

                for (uint h = 0; h != 168; ++h)
                    adq[h] += cluster.series->getAvailablePower(h, year);
            }
        }
    }
}

bool ShouldUseQuadraticOptimisation(const Data::Study& study)
{
    const bool flowQuadEnabled = study.parameters.variablesPrintInfo.isPrinted("FLOW QUAD.");
    if (!flowQuadEnabled)
        return false;

    for (uint j = 0; j < study.runtime->interconnectionsCount(); ++j)
    {
        auto& lnk = *(study.runtime->areaLink[j]);
        auto& impedances = lnk.parameters[Data::fhlImpedances];

        for (uint hour = 0; hour < HOURS_PER_YEAR; ++hour)
        {
            if (Math::Abs(impedances[hour]) >= 1e-100)
            {
                return true;
            }
        }
    }
    return false;
}

void ComputeFlowQuad(Data::Study& study,
                     PROBLEME_HEBDO& problem,
                     const std::vector<AvgExchangeResults*>& balance,
                     uint nbWeeks)
{
    uint startTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;

    if (ShouldUseQuadraticOptimisation(study))
    {
        logs.info() << "Post-processing... (quadratic optimisation)";

        problem.TypeDOptimisation = OPTIMISATION_QUADRATIQUE;
        problem.LeProblemeADejaEteInstancie = false;
        for (uint w = 0; w != nbWeeks; ++w)
        {
            int PasDeTempsDebut = startTime + (w * problem.NombreDePasDeTemps);
            RecalculDesEchangesMoyens(study, problem, balance, PasDeTempsDebut);
        }
    }
    else
    {
        logs.info() << "  The quadratic optimisation has been skipped";

        for (uint j = 0; j < study.runtime->interconnectionsCount(); ++j)
        {
            for (uint w = 0; w != nbWeeks; ++w)
            {
                int PasDeTempsDebut = startTime + (w * problem.NombreDePasDeTemps);
                for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; ++i)
                {
                    const uint indx = i + PasDeTempsDebut;
                    transitMoyenInterconnexionsRecalculQuadratique[j][indx] = 0;
                }
            }
        }
    }
}

void PrepareRandomNumbers(Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          yearRandomNumbers& randomForYear)
{
    uint indexArea = 0;
    study.areas.each([&](Data::Area& area) {
        double rnd = 0.;

        rnd = randomForYear.pUnsuppliedEnergy[indexArea];

        double alea;

        if (area.spreadUnsuppliedEnergyCost == 0)
        {
            if (rnd < 0.5)
                alea = 1.e-4 * (5 + 2 * rnd);
            else
                alea = -1.e-4 * (5 + 2 * (rnd - 0.5));
        }
        else
        {
            alea = (rnd - 0.5) * (area.spreadUnsuppliedEnergyCost);

            if (Math::Abs(alea) < 5.e-4)
            {
                if (alea >= 0)
                    alea += 5.e-4;
                else
                    alea -= 5.e-4;
            }
        }
        problem.CoutDeDefaillancePositive[area.index] = area.thermal.unsuppliedEnergyCost + alea;

        rnd = randomForYear.pSpilledEnergy[indexArea];

        if (area.spreadSpilledEnergyCost == 0)
        {
            if (rnd < 0.5)
                alea = 1.e-4 * (5 + 2 * rnd);
            else
                alea = -1.e-4 * (5 + 2 * (rnd - 0.5));
        }
        else
        {
            alea = (rnd - 0.5) * (area.spreadSpilledEnergyCost);

            if (Math::Abs(alea) < 5.e-4)
            {
                if (alea >= 0)
                    alea += 5.e-4;
                else
                    alea -= 5.e-4;
            }
        }
        problem.CoutDeDefaillanceNegative[area.index] = area.thermal.spilledEnergyCost + alea;


        //-----------------------------
        // Thermal noises
        //-----------------------------
        auto end = area.thermal.list.mapping.end();
        for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
        {
            auto cluster = it->second;
            if (!cluster->enabled)
                continue;
            uint clusterIndex = cluster->areaWideIndex;
            double& rnd = randomForYear.pThermalNoisesByArea[indexArea][clusterIndex];
            double randomClusterProdCost(0.);
            if (cluster->spreadCost == 0) // 5e-4 < |randomClusterProdCost| < 6e-4
            {
                if (rnd < 0.5)
                    randomClusterProdCost = 1e-4 * (5 + 2 * rnd);
                else
                    randomClusterProdCost = -1e-4 * (5 + 2 * (rnd - 0.5));
            }
            else
            {
                randomClusterProdCost = (rnd - 0.5) * (cluster->spreadCost);

                if (Math::Abs(randomClusterProdCost) < 5.e-4)
                {
                    if (Math::Abs(randomClusterProdCost) >= 0)
                        randomClusterProdCost += 5.e-4;
                    else
                        randomClusterProdCost -= 5.e-4;
                }
            }
            rnd = randomClusterProdCost;
        }

        //-----------------------------
        // Hydro noises
        //-----------------------------
        auto& noise = problem.BruitSurCoutHydraulique[area.index];
        switch (study.parameters.power.fluctuations)
        {
        case Data::lssFreeModulations:
        {
            for (uint j = 0; j != 8784; ++j)
                noise[j] = randomForYear.pHydroCostsByArea_freeMod[indexArea][j];

            auto& penalty = problem.CaracteristiquesHydrauliques[area.index];
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations = 5.e-4;
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax = 5.e-4;
            break;
        }

        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            std::fill(noise.begin(), noise.end(), 0);

            auto& penalty = problem.CaracteristiquesHydrauliques[area.index];
            double rnd = randomForYear.pHydroCosts_rampingOrExcursion[indexArea];

            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations
              = 0.01 * (1. + rnd / 10.);
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax
              = 0.1 * (1. + rnd / 100.);
            break;
        }

        case Data::lssUnknown:
        {
            assert(false && "invalid power fluctuations");
            std::fill(noise.begin(), noise.end(), 0);

            auto& penalty = problem.CaracteristiquesHydrauliques[area.index];
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations = 1e-4;
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax = 1e-4;
            break;
        }
        }
        indexArea++;
    });
}

void BuildThermalPartOfWeeklyProblem(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     uint numSpace,
                                     const int PasDeTempsDebut,
                                     double** thermalNoises,
                                     unsigned int year)
{
    int hourInYear = PasDeTempsDebut;
    const uint nbPays = study.areas.size();
    for (unsigned hourInWeek = 0; hourInWeek < problem.NombreDePasDeTemps; ++hourInWeek, ++hourInYear)
    {
        for (uint areaIdx = 0; areaIdx < nbPays; ++areaIdx)
        {
            auto& area = *study.areas.byIndex[areaIdx];
            area.thermal.list.each([&](const Data::ThermalCluster& cluster)
            {
                    auto& Pt = problem.PaliersThermiquesDuPays[areaIdx]
                               .PuissanceDisponibleEtCout[cluster.index];

                    Pt.CoutHoraireDeProductionDuPalierThermique[hourInWeek] =
                        cluster.getMarketBidCost(cluster.series->getSeriesIndex(hourInYear, year), hourInYear)
                        + thermalNoises[areaIdx][cluster.areaWideIndex];

                    Pt.PuissanceDisponibleDuPalierThermique[hourInWeek]
                        = cluster.series->getAvailablePower(hourInYear, year);

                    Pt.PuissanceMinDuPalierThermique[hourInWeek]
                        = (Pt.PuissanceDisponibleDuPalierThermique[hourInWeek] < cluster.PthetaInf[hourInYear])
                        ? Pt.PuissanceDisponibleDuPalierThermique[hourInWeek]
                        : cluster.PthetaInf[hourInYear];
            });
        }
    }

    for (uint k = 0; k < nbPays; ++k)
    {
        auto& area = *study.areas.byIndex[k];

        for (uint l = 0; l != area.thermal.list.size(); ++l)
        {
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[l]
                   .PuissanceDisponibleDuPalierThermiqueRef
            =
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[l]
                   .PuissanceDisponibleDuPalierThermique;
        }
    }

}

int retrieveAverageNTC(const Data::Study& study,
                       const Matrix<>& capacities,
                       const Matrix<uint32_t>& tsNumbers,
                       std::vector<double>& avg)
{
    const auto& parameters = study.parameters;

    const auto& yearsWeight = parameters.getYearsWeight();
    const auto& yearsWeightSum = parameters.getYearsWeightSum();
    const auto& yearsFilter = parameters.yearsFilter;
    const auto width = capacities.width;
    avg.assign(HOURS_PER_YEAR, 0);

    std::map<uint32_t, double> weightOfTS;

    for (uint y = 0; y < study.parameters.nbYears; y++)
    {
        if (!yearsFilter[y])
            continue;

        uint32_t tsIndex = (width == 1) ? 0 : tsNumbers[0][y];
        weightOfTS[tsIndex] += yearsWeight[y];
    }

    // No need for the year number, only the TS index is required
    for (const auto& it : weightOfTS)
    {
        const uint32_t tsIndex = it.first;
        const double weight = it.second;

        for (uint h = 0; h < HOURS_PER_YEAR; h++)
        {
            avg[h] += capacities[tsIndex][h] * weight;
        }
    }

    for (uint h = 0; h < HOURS_PER_YEAR; h++)
    {
        avg[h] /= yearsWeightSum;
    }
    return 0;
}

void finalizeOptimizationStatistics(PROBLEME_HEBDO& problem,
                                    Antares::Solver::Variable::State& state)
{
    auto& firstOptStat = problem.optimizationStatistics[0];
    state.averageOptimizationTime1 = firstOptStat.getAverageSolveTime();
    firstOptStat.reset();

    auto& secondOptStat = problem.optimizationStatistics[1];
    state.averageOptimizationTime2 = secondOptStat.getAverageSolveTime();
    secondOptStat.reset();
}

OptimizationOptions createOptimizationOptions(const Data::Study& study)
{
    return {
        study.parameters.ortoolsUsed,
        study.parameters.ortoolsSolver
    };
}


} // namespace Antares::Solver::Simulation
