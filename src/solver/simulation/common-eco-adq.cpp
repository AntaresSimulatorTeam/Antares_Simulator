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
#include <yuni/core/math.h>
#include <antares/study/study.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/memory-usage.h>
#include <antares/exception/UnfeasibleProblemError.hpp>

#include "common-eco-adq.h"
#include <antares/logs.h>
#include <cassert>
#include "simulation.h"
#include "../aleatoire/alea_fonctions.h"

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace Simulation
{
static void RecalculDesEchangesMoyens(Data::Study& study,
                                      PROBLEME_HEBDO& problem,
                                      const std::vector<AvgExchangeResults*>& balance,
                                      int PasDeTempsDebut)
{
    for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; i++)
    {
        auto& ntcValues = *(problem.ValeursDeNTC[i]);
        uint decalPasDeTemps = PasDeTempsDebut + i;

        for (uint j = 0; j < study.areas.size(); ++j)
        {
            assert(balance[j] && "Impossible to find the variable");
            if (balance[j])
            {
                problem.SoldeMoyenHoraire[i]->SoldeMoyenDuPays[j]
                  = balance[j]->avgdata.hourly[decalPasDeTemps];
            }
            else
            {
                assert(false && "invalid balance");
                problem.SoldeMoyenHoraire[i]->SoldeMoyenDuPays[j] = 0.0;
            }
        }

        std::vector<double> avgDirect;
        std::vector<double> avgIndirect;
        for (uint j = 0; j < study.runtime->interconnectionsCount; ++j)
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

            link->flush();
        }
    }

    try
    {
        OPT_OptimisationHebdomadaire(&problem, 0);
    }
    catch (Data::UnfeasibleProblemError&)
    {
        study.runtime->quadraticOptimizationHasFailed = true;
    }

    for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; ++i)
    {
        const uint indx = i + PasDeTempsDebut;
        auto& ntcValues = *(problem.ValeursDeNTC[i]);
        assert(&ntcValues);

        for (uint j = 0; j < study.runtime->interconnectionsCount; ++j)
        {
            ResultatsParInterconnexion[j]->TransitMoyenRecalculQuadratique[indx]
              = ntcValues.ValeurDuFlux[j];
        }
    }
}

void PrepareDataFromClustersInMustrunMode(Data::Study& study, uint numSpace)
{
    bool inAdequacy = (study.parameters.mode == Data::stdmAdequacy);

    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *study.areas[i];
        auto& scratchpad = *(area.scratchpad[numSpace]);

        memset(scratchpad.mustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);
        if (inAdequacy)
            memset(scratchpad.originalMustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);

        auto& PtChro = *(NumeroChroniquesTireesParPays[numSpace][i]);
        double* mrs = scratchpad.mustrunSum;
        double* adq = scratchpad.originalMustrunSum;

        if (!area.thermal.mustrunList.empty())
        {
            auto end = area.thermal.mustrunList.end();
            for (auto i = area.thermal.mustrunList.begin(); i != end; ++i)
            {
                auto& cluster = *(i->second);
                auto& series = cluster.series->series;
                uint tsIndex = static_cast<uint>(PtChro.ThermiqueParPalier[cluster.areaWideIndex]);
                if (tsIndex >= series.width)
                    tsIndex = 0;

                auto& column = series[tsIndex];

                if (inAdequacy && cluster.mustrunOrigin)
                {
                    for (uint h = 0; h != series.height; ++h)
                    {
                        mrs[h] += column[h];
                        adq[h] += column[h];
                    }
                }
                else
                {
                    for (uint h = 0; h != series.height; ++h)
                        mrs[h] += column[h];
                }

                series.flush();
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

                auto& series = cluster.series->series;
                uint tsIndex = static_cast<uint>(PtChro.ThermiqueParPalier[cluster.areaWideIndex]);
                if (tsIndex >= series.width)
                    tsIndex = 0;

                auto& column = series[tsIndex];
                for (uint h = 0; h != series.height; ++h)
                    adq[h] += column[h];

                series.flush();
            }
        }

        for (uint j = 0; j != area.thermal.clusterCount(); ++j)
        {
            Data::ThermalCluster* cluster = area.thermal.clusters[j];
            cluster->unitCountLastHour[numSpace] = 0;
            cluster->productionLastHour[numSpace] = 0.;
            cluster->pminOfAGroup[numSpace] = 0.;
        }
    }
}

bool ShouldUseQuadraticOptimisation(const Data::Study& study)
{
    uint maxHours = study.runtime->nbHoursPerYear;
    for (uint j = 0; j < study.runtime->interconnectionsCount; ++j)
    {
        auto& lnk = *(study.runtime->areaLink[j]);
        auto& impedances = lnk.parameters[Data::fhlImpedances];

        for (uint hour = 0; hour < maxHours; ++hour)
        {
            if (Math::Abs(impedances[hour]) >= 1e-100)
            {
                lnk.parameters.flush();
                return true;
            }
        }

        lnk.parameters.flush();
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
        problem.LeProblemeADejaEteInstancie = NON_ANTARES;
        for (uint w = 0; w != nbWeeks; ++w)
        {
            int PasDeTempsDebut = startTime + (w * problem.NombreDePasDeTemps);
            RecalculDesEchangesMoyens(study, problem, balance, PasDeTempsDebut);
        }
    }
    else
    {
        logs.info() << "  The quadratic optimisation has been skipped";

        for (uint j = 0; j < study.runtime->interconnectionsCount; ++j)
        {
            for (uint w = 0; w != nbWeeks; ++w)
            {
                int PasDeTempsDebut = startTime + (w * problem.NombreDePasDeTemps);
                for (uint i = 0; i < (uint)problem.NombreDePasDeTemps; ++i)
                {
                    const uint indx = i + PasDeTempsDebut;
                    assert(ResultatsParInterconnexion[j]);
                    ResultatsParInterconnexion[j]->TransitMoyenRecalculQuadratique[indx] = 0;
                    ;
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

        auto* noise = problem.BruitSurCoutHydraulique[area.index];
        switch (study.parameters.power.fluctuations)
        {
        case Data::lssFreeModulations:
        {
            for (uint j = 0; j != 8784; ++j)
                noise[j] = randomForYear.pHydroCostsByArea_freeMod[indexArea][j];

            auto& penalty = *(problem.CaracteristiquesHydrauliques[area.index]);
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations = 5.e-4;
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax = 5.e-4;
            break;
        }

        case Data::lssMinimizeRamping:
        case Data::lssMinimizeExcursions:
        {
            (void)::memset(noise, 0, 8784 * sizeof(double));

            auto& penalty = *(problem.CaracteristiquesHydrauliques[area.index]);
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
            (void)::memset(noise, 0, 8784 * sizeof(double));

            auto& penalty = *(problem.CaracteristiquesHydrauliques[area.index]);
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations = 1e-4;
            penalty.PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax = 1e-4;
            break;
        }
        }
        indexArea++;
    });
}

int retrieveAverageNTC(const Data::Study& study,
                       const Matrix<>& capacities,
                       const Matrix<Yuni::uint32>& tsNumbers,
                       std::vector<double>& avg)
{
    auto yearsWeight = study.parameters.getYearsWeight();
    auto yearsWeightSum = study.parameters.getYearsWeightSum();
    const auto width = capacities.width;
    avg.assign(HOURS_PER_YEAR, 0);

    for (uint y = 0; y < study.parameters.nbYears; y++)
    {
        Yuni::uint32 tsNumber = (width == 1) ? 0 : tsNumbers[0][y];
        for (uint h = 0; h < HOURS_PER_YEAR; h++)
        {
            avg[h] += capacities[tsNumber][h] * yearsWeight[y];
        }
    }

    for (uint h = 0; h < HOURS_PER_YEAR; h++)
    {
        avg[h] /= yearsWeightSum;
    }
    return 0;
}

} // namespace Simulation
} // namespace Solver
} // namespace Antares
