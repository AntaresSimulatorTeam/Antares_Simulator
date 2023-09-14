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
#include <antares/study/study.h>
#include "state.h"

using namespace Yuni;

namespace Antares::Solver::Variable
{
ThermalState::ThermalState(const Data::AreaList& areas)
{
    thermal.resize(areas.size());
    for (const auto& [id, area] : areas)
    {
        assert(area);
        thermal[area->index].initializeFromArea(*area);
    }
}

ThermalState::StateForAnArea& ThermalState::operator[](size_t areaIndex)
{
    return thermal[areaIndex];
}

void ThermalState::StateForAnArea::initializeFromArea(const Data::Area& area)
{
    const auto count = area.thermal.clusterCount();
    thermalClustersProductions.resize(count);
    numberOfUnitsONbyCluster.resize(count);
    thermalClustersOperatingCost.resize(count);
    PMinOfClusters.resize(count);
    unitCountLastHour.resize(count, 0);
    productionLastHour.resize(count, 0);
    pminOfAGroup.resize(count, 0);
}

State::State(Data::Study& s) :
 hourInTheSimulation(0u),
 dispatchableMargin(nullptr),
 studyMode(s.parameters.mode),
 unitCommitmentMode(s.parameters.unitCommitment.ucMode),
 study(s),
 thermal(s.areas),
 simplexHasBeenRan(true),
 annualSystemCost(0.),
 optimalSolutionCost1(0.),
 optimalSolutionCost2(0.),
 averageOptimizationTime1(0.),
 averageOptimizationTime2(0.)
{
}

void State::initFromThermalClusterIndex(const uint clusterAreaWideIndex)
{
    // asserts
    assert(area);
    assert(clusterAreaWideIndex < area->thermal.clusterCount());

    // alias to the current thermal cluster
    thermalCluster = area->thermal.clusters[clusterAreaWideIndex];
    uint serieIndex = timeseriesIndex->ThermiqueParPalier[clusterAreaWideIndex];
    double thermalClusterAvailableProduction
      = thermalCluster->series->timeSeries[serieIndex][hourInTheYear];

    // Minimum power of a group of the cluster for the current hour in the year
    double thermalClusterPMinOfAGroup = 0.;

    if (thermalCluster->mustrun)
    {
        // When the cluster is in must-run mode, the production value
        // directly comes from the time-series
        // it doen't exist from the solver perspective
        assert(thermalCluster->series);
        assert(timeseriesIndex != NULL);
        assert(hourInTheYear < thermalCluster->series->timeSeries.height);

        thermal[area->index].thermalClustersProductions[clusterAreaWideIndex]
          = thermalClusterAvailableProduction;

        thermal[area->index].PMinOfClusters[clusterAreaWideIndex] = 0.;
        thermal[area->index].numberOfUnitsONbyCluster[clusterAreaWideIndex]
          = 0; // will be calculated during the smoothing
    }
    else
    {
        // otherwise from the solver (most of the time)

        if (studyMode != Data::stdmAdequacy) // Economy
        {
            thermalClusterPMinOfAGroup
              = problemeHebdo->PaliersThermiquesDuPays[area->index]
                  .pminDUnGroupeDuPalierThermique[thermalCluster->index]; // one by cluster
            thermal[area->index].PMinOfClusters[clusterAreaWideIndex]
              = problemeHebdo->PaliersThermiquesDuPays[area->index]
                  .PuissanceDisponibleEtCout[thermalCluster->index]
                  .PuissanceMinDuPalierThermique[hourInTheWeek]; // one per hour for one
            // cluster
        }
        else
        {
            // Adequacy
            thermal[area->index].PMinOfClusters[clusterAreaWideIndex] = 0.;
        }

        thermal[area->index].thermalClustersProductions[clusterAreaWideIndex]
          = hourlyResults->ProductionThermique[hourInTheWeek]
              .ProductionThermiqueDuPalier[thermalCluster->index];

        if (unitCommitmentMode == Antares::Data::UnitCommitmentMode::ucMILP) // Economy accurate
            thermal[area->index].numberOfUnitsONbyCluster[clusterAreaWideIndex]
              = static_cast<uint>(hourlyResults->ProductionThermique[hourInTheWeek]
                                    .NombreDeGroupesEnMarcheDuPalier[thermalCluster->index]);
        else
            // Economy Fast or Adequacy -- will be calculated during the smoothing
            thermal[area->index].numberOfUnitsONbyCluster[clusterAreaWideIndex] = 0;
    }

    initFromThermalClusterIndexProduction(clusterAreaWideIndex);

    if (studyMode != Data::stdmAdequacy)
    {
        // Minimum power of a group of the cluster, one per year for each cluster - from the
        // solver
        thermal[area->index].pminOfAGroup[clusterAreaWideIndex] = thermalClusterPMinOfAGroup;
    }

    // Nombre min de groupes appelés
    // en mode accurate : est pris depuis le solveur
    // en mode fast : est pris depuis l'heuristique
}

void State::initFromThermalClusterIndexProduction(const uint clusterAreaWideIndex)
{
    uint serieIndex = timeseriesIndex->ThermiqueParPalier[clusterAreaWideIndex];

    if (thermal[area->index].thermalClustersProductions[clusterAreaWideIndex] > 0.)
    {
        // alias to the production of the current thermal cluster
        double p = thermal[area->index].thermalClustersProductions[clusterAreaWideIndex];
        // alias to the previous number of started units
        uint previousUnitCount = thermal[area->index].unitCountLastHour[clusterAreaWideIndex];

        // Looking for the new number of units which have been started
        uint newUnitCount;

        if (p > thermal[area->index].productionLastHour[clusterAreaWideIndex])
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
                newUnitCount
                  = static_cast<uint>(Math::Ceil(p / thermalCluster->nominalCapacityWithSpinning));
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
        thermal[area->index].thermalClustersOperatingCost[clusterAreaWideIndex]
          = (p * thermalCluster->getOperatingCost(serieIndex, hourInTheYear));

        // Startup cost
        if (newUnitCount > previousUnitCount && hourInTheSimulation != 0u)
        {
            thermal[area->index].thermalClustersOperatingCost[clusterAreaWideIndex]
              += thermalCluster->startupCost * (newUnitCount - previousUnitCount);
        }

        // Fixed price
        thermal[area->index].thermalClustersOperatingCost[clusterAreaWideIndex]
          += thermalCluster->fixedCost * newUnitCount;

        // Storing the new unit count for the next hour
        thermal[area->index].unitCountLastHour[clusterAreaWideIndex] = newUnitCount;
        thermal[area->index].productionLastHour[clusterAreaWideIndex] = p;
    }
    else
    {
        thermal[area->index].thermalClustersOperatingCost[clusterAreaWideIndex] = 0.;
        thermal[area->index].unitCountLastHour[clusterAreaWideIndex] = 0u;
        thermal[area->index].productionLastHour[clusterAreaWideIndex] = 0.;
    }
}

void State::yearEndBuildFromThermalClusterIndex(const uint clusterAreaWideIndex)
{
    uint maxDurationON;    // nombre d'heures de fonctionnement d'un groupe au delà duquel un
    // arrêt/redémarrage est préférable
    uint maxUnitNeeded = 0;
    uint startHourForCurrentYear = study.runtime->rangeLimits.hour[Data::rangeBegin];
    uint endHourForCurrentYear
        = startHourForCurrentYear + study.runtime->rangeLimits.hour[Data::rangeCount];

    assert(endHourForCurrentYear <= Variable::maxHoursInAYear);

    // Nombre minimal de groupes en fonctionnement à l'heure h (determiné par Peff  et Pnom)
    std::array<uint, Variable::maxHoursInAYear> ON_min;
    // Nombre maximal de groupes en fonctionnement à l'heure h  (determine par Peff et Pmin)
    std::array<uint, Variable::maxHoursInAYear> ON_max;
    // Nombre de groupes économiquement optimal en fonctionnement à l'heure h
    std::array<uint, Variable::maxHoursInAYear> ON_opt {};


    // Get cluster properties
    Data::ThermalCluster* currentCluster = area->thermal.clusters[clusterAreaWideIndex];
    uint serieIndex = timeseriesIndex->ThermiqueParPalier[clusterAreaWideIndex];

    assert(endHourForCurrentYear <= Variable::maxHoursInAYear);
    assert(endHourForCurrentYear <= currentCluster->series->timeSeries.height);
    assert(currentCluster);
    assert(currentCluster->series);
    assert(timeseriesIndex != NULL);

    if (currentCluster->fixedCost > 0.)
    {
        maxDurationON = static_cast<uint>(
                Math::Floor(currentCluster->startupCost / currentCluster->fixedCost));
        if (maxDurationON > endHourForCurrentYear)
            maxDurationON = endHourForCurrentYear;
    }
    else
        maxDurationON = endHourForCurrentYear;

    // min, and max unit ON calculation
    for (uint h = startHourForCurrentYear; h < endHourForCurrentYear; ++h)
    {
        maxUnitNeeded = 0u;
        ON_min[h] = 0u;
        ON_max[h] = 0u;

        // Getting available production from cluster data
        double thermalClusterAvailableProduction = currentCluster->series->timeSeries[serieIndex][h];
        double thermalClusterProduction = 0.;
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

        if (thermalClusterProduction <= 0.)
            continue;

        thermalClusterOperatingCostForYear[h]
          = thermalClusterProduction * currentCluster->getOperatingCost(serieIndex, h);

        switch (unitCommitmentMode)
        {
            case Antares::Data::UnitCommitmentMode::ucHeuristic:
                {
                    //	ON_min[h] = static_cast<uint>(Math::Ceil(thermalClusterProduction /
                    // currentCluster->nominalCapacityWithSpinning)); // code 5.0.3b<7
                    // 5.0.3b7
                    if (thermal[area->index].pminOfAGroup[clusterAreaWideIndex] > 0.)
                    {
                        ON_min[h] = Math::Max(
                                Math::Min(static_cast<uint>(
                                        Math::Floor(thermalClusterPMinOfTheClusterForYear[h]
                                            / thermal[area->index].pminOfAGroup[clusterAreaWideIndex])),
                                    static_cast<uint>(
                                        Math::Ceil(thermalClusterAvailableProduction
                                            / currentCluster->nominalCapacityWithSpinning))),
                                static_cast<uint>(
                                    Math::Ceil(thermalClusterProduction / currentCluster->nominalCapacityWithSpinning)));
                    }
                    else
                        ON_min[h] = static_cast<uint>(Math::Ceil(
                                    thermalClusterProduction / currentCluster->nominalCapacityWithSpinning));
                    break;
                }
            case Antares::Data::UnitCommitmentMode::ucMILP:
                {
                    ON_min[h] = Math::Max(
                            static_cast<uint>(Math::Ceil(thermalClusterProduction / currentCluster->nominalCapacityWithSpinning)),
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

    if (maxDurationON > 0)
        ON_opt = computeEconomicallyOptimalNbClustersONforEachHour(maxDurationON, ON_min, ON_max);

    // Calculation of non linear and startup costs
    yearEndBuildThermalClusterCalculateStartupCosts(maxDurationON, ON_min, ON_opt, currentCluster);
}

void State::yearEndBuildThermalClusterCalculateStartupCosts(const uint& maxDurationON,
                                                            const std::array<uint, Variable::maxHoursInAYear>& ON_min,
                                                            const std::array<uint, Variable::maxHoursInAYear>& ON_opt,
                const Data::ThermalCluster* currentCluster)
{
    uint startHourForCurrentYear = study.runtime->rangeLimits.hour[Data::rangeBegin];
    uint endHourForCurrentYear
        = startHourForCurrentYear + study.runtime->rangeLimits.hour[Data::rangeCount];

    for (uint hour = startHourForCurrentYear; hour < endHourForCurrentYear; ++hour)
    {
        double thermalClusterStartupCostForYear = 0;
        double thermalClusterFixedCostForYear = 0;

        // based on duration, if maxDurationON==0 we choose the mininum of ON clusters, otherwise, the
        // optimal number.
        uint optimalCount = (maxDurationON == 0) ? ON_min[hour] : ON_opt[hour];

        // NODU cannot be > unit count
        if (optimalCount > currentCluster->unitCount)
            optimalCount = currentCluster->unitCount;

        thermalClusterFixedCostForYear = currentCluster->fixedCost * optimalCount;

        if (hour >= startHourForCurrentYear + 1) // starting hour +1 (fron start hour)
        {
            // nombre de groupes démarrés à l'heure h
            int delta = (maxDurationON == 0) ? ON_min[hour] - ON_min[hour - 1]
                : ON_opt[hour] - ON_opt[hour - 1];

            (delta > 0)
                ? (thermalClusterStartupCostForYear = currentCluster->startupCost * delta)
                : (thermalClusterStartupCostForYear = 0.);
        }

        // Aggregated variables for output
        // NP Cost = SU + Fx
        // Op. Cost = (P.lvl * P.Cost) + NP.Cost

        thermalClusterNonProportionalCostForYear[hour]
            = thermalClusterStartupCostForYear + thermalClusterFixedCostForYear;
        thermalClusterOperatingCostForYear[hour] += thermalClusterNonProportionalCostForYear[hour];

        // Other variables for output
        //\todo get from the cluster
        thermalClusterDispatchedUnitsCountForYear[hour] = optimalCount;
    }
}

std::array<uint, Variable::maxHoursInAYear>
State::computeEconomicallyOptimalNbClustersONforEachHour(const uint& maxDurationON,
                  const std::array<uint, Variable::maxHoursInAYear>& ON_min,
                  const std::array<uint, Variable::maxHoursInAYear>& ON_max) const
{
    uint startHourForCurrentYear = study.runtime->rangeLimits.hour[Data::rangeBegin];
    uint endHourForCurrentYear
        = startHourForCurrentYear + study.runtime->rangeLimits.hour[Data::rangeCount];

    // Nombre de groupes économiquement optimal en fonctionnement à l'heure h
    std::array<uint, Variable::maxHoursInAYear> ON_opt;

    uint nivmax; // valeur maximale de ON_opt[h] , progressivement réactualisée à la baisse
    uint nivmin; // valeur minimale de ON_opt[h] , progressivement réactualisée à la hausse

    ON_opt[startHourForCurrentYear] = ON_min[startHourForCurrentYear];

    uint h = startHourForCurrentYear + 1;
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
            // nombre de pas de temps sur lesquel la valeur de ON_opt[h] est maintenue
            uint portee = 0;
            nivmin = ON_min[h];

            (ON_max[h] < ON_opt[h - 1]) ? nivmax = ON_max[h] : nivmax = ON_opt[h - 1];

            if (nivmax > nivmin)
            {
                for (uint k = 1; k <= maxDurationON; ++k)
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
                for (uint k = 0; k < portee; ++k)
                {
                    ON_opt[h + k] = nivmin;
                }
                h += portee; // on progresse d'au moins une heure
            }
        }
    }
    return ON_opt;
}

} // namespace Antares::Solver::Variable
