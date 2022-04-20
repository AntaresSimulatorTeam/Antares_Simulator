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
#include <antares/study.h>
#include "state.h"

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace Variable
{
State::State(Data::Study& s) :
 hourInTheSimulation(0u),
 dispatchableMargin(nullptr),
 studyMode(s.parameters.mode),
 unitCommitmentMode(s.parameters.unitCommitment.ucMode),
 study(s),
 simplexHasBeenRan(true),
 annualSystemCost(0.),
 optimalSolutionCost1(0.),
 optimalSolutionCost2(0.),
 averageOptimizationTime(0.)
{
    h2oValueWorkVars.levelUp = 0.;
    h2oValueWorkVars.levelDown = 0.;
}

void State::initFromThermalClusterIndex(const uint clusterAreaWideIndex, uint numSpace)
{
    // asserts
    assert(area);
    assert(clusterAreaWideIndex < area->thermal.clusterCount());

    thermalClusterNonProportionalCost = 0.;

    // alias to the current thermal cluster
    thermalCluster = area->thermal.clusters[clusterAreaWideIndex];
    // Getting its production (only if in economy mode)
    if (studyMode != Data::stdmAdequacyDraft)
    {
        uint serieIndex = timeseriesIndex->ThermiqueParPalier[clusterAreaWideIndex];
        thermalClusterAvailableProduction
          = thermalCluster->series->series[serieIndex][hourInTheYear];

        if (thermalCluster->mustrun)
        {
            // When the cluster is in must-run mode, the production value
            // directly comes from the time-series
            // it doen't exist from the solver perspective
            assert(thermalCluster->series);
            assert(timeseriesIndex != NULL);
            assert(hourInTheYear < thermalCluster->series->series.height);

            thermalClusterProduction = thermalClusterAvailableProduction;

            thermalClusterPMinOfAGroup = 0.;
            thermalClusterPMinOfTheCluster = 0.;
            thermalClusterNumberON = 0; // will be calculated during the smoothing
        }
        else
        {
            // otherwise from the solver (most of the time)

            if (studyMode != Data::stdmAdequacy) // Economy
            {
                thermalClusterPMinOfAGroup
                  = problemeHebdo->PaliersThermiquesDuPays[area->index]
                      ->PminDUnGroupeDuPalierThermique[thermalCluster->index]; // one by cluster
                thermalClusterPMinOfTheCluster
                  = problemeHebdo->PaliersThermiquesDuPays[area->index]
                      ->PuissanceDisponibleEtCout[thermalCluster->index]
                      ->PuissanceMinDuPalierThermique[hourInTheWeek]; // one per hour for one
                                                                      // cluster
            }
            else
            {
                // Adequacy
                thermalClusterPMinOfAGroup = 0.;
                thermalClusterPMinOfTheCluster = 0.;
            }

            thermalClusterProduction = hourlyResults->ProductionThermique[hourInTheWeek]
                                         ->ProductionThermiqueDuPalier[thermalCluster->index];

            if (unitCommitmentMode == Antares::Data::UnitCommitmentMode::ucMILP) // Economy accurate
                thermalClusterNumberON
                  = static_cast<uint>(hourlyResults->ProductionThermique[hourInTheWeek]
                                        ->NombreDeGroupesEnMarcheDuPalier[thermalCluster->index]);
            else
                thermalClusterNumberON
                  = 0; // Economy Fast or Adequacy -- will be calculated during the smoothing
        }

        // The operating cost, for a single cluster of a single area
        //
        // Reminder :
        //   The current area    : state.area
        //   The current cluster : state.thermalCluster
        //
        // --- v3.4 ---
        //  O(h) = MB * P(h)
        // which was merely the following expression :
        // (state.aggregate->productionCost[state.hourInTheYear] * state.thermalClusterProduction);
        // OperatingCost = OperatingCost for the thermal cluster/Mwh * production
        //
        // --- v3.5 ---
        // Marginal (€/MWh)     MA
        // Spread (€/MWh)       SP
        // Fixed (€ / hour)     FI
        // Start-up (€/start)   SU
        // Market bid (€/ MWh)  MB
        //
        // P : production of the current cluster for the current hour
        // Unom : nominal hourly power for one unit
        // N(h) = ceil(P / Unom) : minimal number of unit used for the current hour
        // O = result for the current hour
        //
        // O(h) = MA * P(h) * Modulation
        // if (P > 0)
        //     O += FI * unit
        // if (N(h) > N(h-1))
        //     O += SU * [N(h) - N(h-1)]
        //
        // \see 101206-antares-couts.doc
        //
        // --- v3.7 ---
        // New algorithm
        //
        // Reminder: the variable 'productionCost' is a vector only valid when used
        //   from the solver, which is, for each hour in the year, the product
        //   of the market bid price with the modulation vector

        if (thermalClusterProduction > 0.)
        {
            // alias to the production of the current thermal cluster
            register double p = thermalClusterProduction;
            // alias to the previous number of started units
            register uint previousUnitCount = thermalCluster->unitCountLastHour[numSpace];

            // Looking for the new number of units which have been started
            uint newUnitCount;

            if (p > thermalCluster->productionLastHour[numSpace])
            {
                newUnitCount
                  = static_cast<uint>(Math::Ceil(p / thermalCluster->nominalCapacityWithSpinning));
                if (newUnitCount > thermalCluster->unitCount)
                    newUnitCount = thermalCluster->unitCount;
                if (newUnitCount < previousUnitCount)
                    newUnitCount = previousUnitCount;
            }
            else
            {
                if (thermalCluster->minStablePower > 0.)
                {
                    newUnitCount = static_cast<uint>(
                      Math::Ceil(p / thermalCluster->nominalCapacityWithSpinning));
                    if (newUnitCount > thermalCluster->unitCount)
                        newUnitCount = thermalCluster->unitCount;
                }
                else
                    newUnitCount = thermalCluster->unitCount;

                if (newUnitCount > previousUnitCount)
                    newUnitCount = previousUnitCount;
            }

            // calculating the operating cost for the current hour
            // O(h) = MA * P(h) * Modulation
            assert(thermalCluster->productionCost != NULL && "invalid production cost");
            thermalClusterOperatingCost = (p * thermalCluster->productionCost[hourInTheYear]);

            // Startup cost
            if (newUnitCount > previousUnitCount && hourInTheSimulation != 0u)
            {
                thermalClusterOperatingCost
                  += thermalCluster->startupCost * (newUnitCount - previousUnitCount);
                thermalClusterNonProportionalCost
                  = thermalCluster->startupCost * (newUnitCount - previousUnitCount);
            }

            // Fixed price
            thermalClusterOperatingCost += thermalCluster->fixedCost * newUnitCount;
            thermalClusterNonProportionalCost += thermalCluster->fixedCost * newUnitCount;

            // Storing the new unit count for the next hour
            thermalCluster->unitCountLastHour[numSpace] = newUnitCount;
            thermalCluster->productionLastHour[numSpace] = p;
        }
        else
        {
            thermalClusterOperatingCost = 0.;
            thermalClusterNonProportionalCost = 0.;
            thermalCluster->unitCountLastHour[numSpace] = 0u;
            thermalCluster->productionLastHour[numSpace] = 0.;
        }

        if (studyMode != Data::stdmAdequacy)
        {
            // Minimum power of a group of the cluster, one per year for each cluster - from the
            // solver
            thermalCluster->pminOfAGroup[numSpace] = thermalClusterPMinOfAGroup;
        }

        // Nombre min de groupes appelés
        // en mode accurate : est pris depuis le solveur
        // en mode fast : est pris depuis l'heuristique
    }
}

void State::initFromRenewableClusterIndex(const uint clusterAreaWideIndex, uint /* numSpace */)
{
    assert(area);
    assert(clusterAreaWideIndex < area->renewable.clusterCount());

    // alias to the current renewable cluster
    renewableCluster = area->renewable.clusters[clusterAreaWideIndex];
    assert(timeseriesIndex);
    uint serieIndex = timeseriesIndex->RenouvelableParPalier[clusterAreaWideIndex];

    assert(renewableCluster->series);
    renewableClusterProduction = renewableCluster->valueAtTimeStep(serieIndex, hourInTheYear);
}

void State::yearEndBuildFromThermalClusterIndex(const uint clusterAreaWideIndex, uint numSpace)
{
    if (studyMode != Data::stdmAdequacyDraft)
    {
        uint dur;    // nombre d'heures de fonctionnement d'un groupe au delà duquel un
                     // arrêt/redémarrage est préférable
        uint nivmax; // valeur maximale de ON_opt[h] , progressivement réactualisée à la baisse
        uint nivmin; // valeur minimale de ON_opt[h] , progressivement réactualisée à la hausse
        uint portee; // nombre de pas de temps sur lesquel la valeur de ON_opt[h] est maintenue
                     // identique
        int delta;   // nombre de groupes démarrés à l'heure h
        uint maxUnitNeeded = 0;
        uint k = 0;
        uint h = 0;
        uint i = 0;
        uint optimalCount;
        uint startHourForCurrentYear = study.runtime->rangeLimits.hour[Data::rangeBegin];
        uint endHourForCurrentYear
          = startHourForCurrentYear + study.runtime->rangeLimits.hour[Data::rangeCount];

        assert(endHourForCurrentYear <= Variable::maxHoursInAYear);
        uint ON_min[Variable::maxHoursInAYear] = {}; // Nombre minimal de groupes en fonctionnement
                                                     // à l'heure h (determiné par Peff  et Pnom)
        uint ON_max[Variable::maxHoursInAYear] = {}; // Nombre maximal de groupes en fonctionnement
                                                     // à l'heure h  (determine par Peff et Pmin)
        uint ON_opt[Variable::maxHoursInAYear]
          = {}; // Nombre de groupes économiquement optimal en fonctionnement à l'heure h

        double thermalClusterStartupCostForYear[Variable::maxHoursInAYear]
          = {}; // Starup cost à l'heure h
        double thermalClusterFixedCostForYear[Variable::maxHoursInAYear] = {};

        // Get cluster properties
        Data::ThermalCluster* currentCluster = area->thermal.clusters[clusterAreaWideIndex];
        uint serieIndex = timeseriesIndex->ThermiqueParPalier[clusterAreaWideIndex];

        assert(endHourForCurrentYear <= Variable::maxHoursInAYear);
        assert(endHourForCurrentYear <= currentCluster->series->series.height);
        assert(currentCluster);
        assert(currentCluster->series);
        assert(timeseriesIndex != NULL);

        if (currentCluster->fixedCost > 0.)
        {
            dur = static_cast<uint>(
              Math::Floor(currentCluster->startupCost / currentCluster->fixedCost));
            if (dur > endHourForCurrentYear)
                dur = endHourForCurrentYear;
        }
        else
            dur = endHourForCurrentYear;

        // min, and max unit ON calculation
        for (h = startHourForCurrentYear; h < endHourForCurrentYear; ++h)
        {
            maxUnitNeeded = 0u;
            ON_min[h] = 0u;
            ON_max[h] = 0u;

            // Getting available production from cluster data
            thermalClusterAvailableProduction = currentCluster->series->series[serieIndex][h];

            if (currentCluster->mustrun)
            {
                // When the cluster is in must-run mode, the production value
                // directly comes from the time-series
                thermalClusterProduction
                  = thermalClusterAvailableProduction; // in mustrun, production==available
                                                       // production
            }
            else
            {
                // otherwise from the solver results (most of the time)
                thermalClusterProduction = thermalClusterProductionForYear[h];
            }

            if (thermalClusterProduction > 0.)
            {
                thermalClusterOperatingCostForYear[h]
                  = (thermalClusterProduction * currentCluster->productionCost[h]);

                switch (unitCommitmentMode)
                {
                case Antares::Data::UnitCommitmentMode::ucHeuristic:
                {
                    /*if (thermalClusterPMinOfAGroup > 0.) // code 5.0.2
                    {
                            ON_min[h] = Math::Max(
                                                    static_cast<uint>(Math::Ceil(thermalClusterProduction
                    / currentCluster->nominalCapacityWithSpinning)),
                                                    static_cast<uint>(Math::Ceil(thermalClusterPMinOfTheClusterForYear[h]
                    / currentCluster->pminOfAGroup)) );

                    }
                    else*/
                    //	ON_min[h] = static_cast<uint>(Math::Ceil(thermalClusterProduction /
                    // currentCluster->nominalCapacityWithSpinning)); // code 5.0.3b<7
                    // 5.0.3b7
                    if (currentCluster->pminOfAGroup[numSpace] > 0.)
                    {
                        ON_min[h] = Math::Max(
                          Math::Min(static_cast<uint>(
                                      Math::Floor(thermalClusterPMinOfTheClusterForYear[h]
                                                  / currentCluster->pminOfAGroup[numSpace])),
                                    static_cast<uint>(
                                      Math::Ceil(thermalClusterAvailableProduction
                                                 / currentCluster->nominalCapacityWithSpinning))),
                          static_cast<uint>(
                            Math::Ceil(thermalClusterProduction
                                       / currentCluster->nominalCapacityWithSpinning)));
                    }
                    else
                        ON_min[h] = static_cast<uint>(Math::Ceil(
                          thermalClusterProduction / currentCluster->nominalCapacityWithSpinning));
                    break;
                }
                case Antares::Data::UnitCommitmentMode::ucMILP:
                {
                    ON_min[h] = Math::Max(
                      static_cast<uint>(Math::Ceil(thermalClusterProduction
                                                   / currentCluster->nominalCapacityWithSpinning)),
                      thermalClusterDispatchedUnitsCountForYear[h]); // eq. to thermalClusterON for
                                                                     // that hour

                    break;
                }
                case Antares::Data::UnitCommitmentMode::ucUnknown:
                {
                    logs.warning() << "Unknown unit-commitment mode";
                    break;
                }
                }

                ON_max[h] = static_cast<uint>(Math::Ceil(
                  thermalClusterAvailableProduction / currentCluster->nominalCapacityWithSpinning));

                if (currentCluster->minStablePower > 0.)
                {
                    maxUnitNeeded = static_cast<uint>(
                      Math::Floor(thermalClusterProduction / currentCluster->minStablePower));
                    if (ON_max[h] > maxUnitNeeded)
                        ON_max[h] = maxUnitNeeded;
                }

                if (ON_max[h] < ON_min[h])
                    ON_max[h] = ON_min[h];
            }
        }

        if (dur > 0)
        {
            ON_opt[startHourForCurrentYear] = ON_min[startHourForCurrentYear];
            h = startHourForCurrentYear + 1;

            while (h < endHourForCurrentYear)
            {
                if (ON_min[h] >= ON_opt[h - 1])
                {
                    ON_opt[h] = ON_min[h]; // à la montée le nombre de groupe démarré est le plus
                                           // petit possible
                    ++h; // à la montée on ne peut progresser que d'une heure
                }
                else // on amorce une descente : ON_opt[h] peut être supérieur à  ON_min[h]
                {
                    portee = 0;
                    nivmin = ON_min[h];

                    (ON_max[h] < ON_opt[h - 1]) ? nivmax = ON_max[h] : nivmax = ON_opt[h - 1];

                    if (nivmax > nivmin)
                    {
                        for (k = 1; k <= dur; ++k)
                        {
                            if (h + k >= endHourForCurrentYear)
                                break; // fin de l'année dépassée
                            if (ON_max[h + k] <= ON_min[h])
                            {
                                nivmax = ON_min[h];
                                break;
                            } // point très  bas rencontré sur ON_max : il vaut mieux arrêter les
                              // groupes dès l'heure h
                            if (ON_max[h + k]
                                < nivmax) // point moins bas rencontré sur ON_max : la borne sup du
                                          // nombre optimal de groupes à conserver en h diminue
                            {
                                nivmax = ON_max[h + k];
                                if (nivmax < nivmin)
                                    break;
                            }
                            if (ON_min[h + k] > ON_min[h]) // on est sûr que ON_opt[h] > ON_min[h]
                            {
                                if (ON_min[h + k] >= nivmax)
                                {
                                    nivmin = nivmax;
                                    portee = k;
                                    break;
                                } // la remontée de ON_min justifie de conserver exactement
                                  // nivmin=nivmax groupes de h à h+k-1 = h+portee-1
                                else if (ON_min[h + k] >= nivmin)
                                {
                                    portee = k; // durée  provisoire qui pourra être allongée
                                    nivmin
                                      = ON_min[h + k]; // niveau provisoire qui pourra être augmenté
                                }
                            }
                        }
                    }

                    if (portee == 0)
                    {
                        ON_opt[h] = ON_min[h]; // la puissance appelée après h ne justifie pas de
                                               // maintenir des groupes appelés au-delà du minimum
                        ++h;                   // on progresse d'exactement une heure
                    }
                    else
                    {
                        for (k = 0; k < portee; ++k)
                        {
                            ON_opt[h + k] = nivmin;
                        }
                        h += portee; // on progresse d'au moins une heure
                    }
                }
            }
        }

        // Calculation of non linear and startup costs
        for (i = startHourForCurrentYear; i < endHourForCurrentYear; ++i)
        {
            // based on duration, if dur==0 we choose the mininum of ON clusters, otherwise, the
            // optimal number.
            (dur == 0) ? (optimalCount = ON_min[i]) : (optimalCount = ON_opt[i]);

            // NODU cannot be > unit count
            if (optimalCount > currentCluster->unitCount)
                optimalCount = currentCluster->unitCount;

            thermalClusterFixedCostForYear[i] = currentCluster->fixedCost * optimalCount;

            if (i >= startHourForCurrentYear + 1) // starting hour +1 (fron start hour)
            {
                (dur == 0) ? (delta = ON_min[i] - ON_min[i - 1])
                           : (delta = ON_opt[i] - ON_opt[i - 1]);

                (delta > 0)
                  ? (thermalClusterStartupCostForYear[i] = currentCluster->startupCost * delta)
                  : (thermalClusterStartupCostForYear[i] = 0.);
            }

            // Aggregated variables for output
            // NP Cost = SU + Fx
            // Op. Cost = (P.lvl * P.Cost) + NP.Cost

            thermalClusterNonProportionalCostForYear[i]
              = thermalClusterStartupCostForYear[i] + thermalClusterFixedCostForYear[i];
            thermalClusterOperatingCostForYear[i] += thermalClusterNonProportionalCostForYear[i];

            // Other variables for output
            //\todo get from the cluster
            thermalClusterDispatchedUnitsCountForYear[i] = optimalCount;
        }
    }
}

} // namespace Variable
} // namespace Solver
} // namespace Antares
