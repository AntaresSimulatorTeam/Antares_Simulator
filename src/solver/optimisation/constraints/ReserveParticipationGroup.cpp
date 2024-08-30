/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/constraints/ReserveParticipationGroup.h"
#include "antares/solver/optimisation/constraints/ConstraintGroup.h"
#include "antares/solver/optimisation/constraints/PMaxReserve.h"
#include "antares/solver/optimisation/constraints/POutCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/PRunningUnits.h"
#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"
#include "antares/solver/optimisation/constraints/POutBounds.h"
#include "antares/solver/optimisation/constraints/STTurbiningMaxReserve.h"
#include "antares/solver/optimisation/constraints/STPumpingMaxReserve.h"
#include "antares/solver/optimisation/constraints/STTurbiningCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/STPumpingCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/STReserveUpParticipation.h"
#include "antares/solver/optimisation/constraints/STReserveDownParticipation.h"
#include "antares/solver/optimisation/constraints/LTTurbiningMaxReserve.h"
#include "antares/solver/optimisation/constraints/LTPumpingMaxReserve.h"
#include "antares/solver/optimisation/constraints/LTReserveUpParticipation.h"
#include "antares/solver/optimisation/constraints/LTReserveDownParticipation.h"
#include "antares/solver/optimisation/constraints/LTTurbiningCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/LTPumpingCapacityThreasholds.h"
                               

ReserveParticipationGroup::ReserveParticipationGroup(PROBLEME_HEBDO* problemeHebdo,
                                                     bool simulation,
                                                     ConstraintBuilder& builder) :
 AbstractStartUpCostsGroup(problemeHebdo, simulation, builder)
{
    this->simulation_ = simulation;
}

ReserveData ReserveParticipationGroup::GetReserveDataFromProblemHebdo()
{
    return {.Simulation = simulation_,
            .areaReserves = problemeHebdo_->allReserves,
            .thermalClusters = problemeHebdo_->PaliersThermiquesDuPays,
            .shortTermStorageOfArea = problemeHebdo_->ShortTermStorage,
            .longTermStorageOfArea = problemeHebdo_->LongTermStorage,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build MinDownTime constraints with
 * respect to default order
 */
void ReserveParticipationGroup::BuildConstraints()
{
    {
        auto data = GetReserveDataFromProblemHebdo();
        PMaxReserve pMaxReserve(builder_, data);
        PRunningUnits pRunningUnits(builder_, data);
        ReserveSatisfaction reserveSatisfaction(builder_, data);
        STTurbiningMaxReserve STTurbiningMaxReserve(builder_, data);
        STPumpingMaxReserve STPumpingMaxReserve(builder_, data);
        STReserveUpParticipation STReserveUpParticipation(builder_, data);
        STReserveDownParticipation STReserveDownParticipation(builder_, data);
        LTTurbiningMaxReserve LTTurbiningMaxReserve(builder_, data);
        LTPumpingMaxReserve LTPumpingMaxReserve(builder_, data);
        LTReserveUpParticipation LTReserveUpParticipation(builder_, data);
        LTReserveDownParticipation LTReserveDownParticipation(builder_, data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            // Adding constraints for ReservesUp and ReservesDown
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                // Thermal clusters reserve participations
                {
                    auto& areaReservesUp = data.areaReserves[pays].areaCapacityReservationsUp;
                    uint32_t reserve = 0;
                    for (const auto& areaReserveUp : areaReservesUp)
                    {
                        // 24
                        reserveSatisfaction.add(pays, reserve, pdt, true);

                        uint32_t cluster_participation = 0;
                        for (const auto& clusterReserveParticipation :
                             areaReserveUp.AllThermalReservesParticipation)
                        {
                            if (clusterReserveParticipation.maxPower >= 0)
                            {
                                // 16 bis
                                pMaxReserve.add(pays, reserve, cluster_participation, pdt, true);

                                // 17 quater
                                pRunningUnits.add(pays, reserve, cluster_participation, pdt, true);
                            }
                            cluster_participation++;
                        }
                        reserve++;
                    }

                    reserve = 0;
                    auto& areaReservesDown = data.areaReserves[pays].areaCapacityReservationsDown;
                    for (const auto& areaReserveDown : areaReservesDown)
                    {
                        // 24
                        reserveSatisfaction.add(pays, reserve, pdt, false);

                        uint32_t cluster_participation = 0;
                        for (const auto& clusterReserveParticipation :
                             areaReserveDown.AllThermalReservesParticipation)
                        {
                            if (clusterReserveParticipation.maxPower > 0)
                            {
                                // 16 bis
                                pMaxReserve.add(pays, reserve, cluster_participation, pdt, false);

                                // 17 quater
                                pRunningUnits.add(pays, reserve, cluster_participation, pdt, false);
                            }
                            cluster_participation++;
                        }
                        reserve++;
                    }
                }

                // ShortTerm Storage reserve participations
                {
                    auto& areaReservesUp = data.areaReserves[pays].areaCapacityReservationsUp;
                    uint32_t reserve = 0;
                    for (const auto& areaReserveUp : areaReservesUp)
                    {
                        uint32_t cluster_participation = 0;
                        for (const auto& clusterReserveParticipation :
                             areaReserveUp.AllSTStorageReservesParticipation)
                        {
                            if (clusterReserveParticipation.maxTurbining > 0)
                            {
                                // 15 (k)
                                STTurbiningMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, true);
                            }
                            if (clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (l)
                                STPumpingMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, true);
                            }
                            if (clusterReserveParticipation.maxTurbining > 0
                                || clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (o)
                                STReserveUpParticipation.add(
                                  pays, reserve, cluster_participation, pdt);
                            }
                            cluster_participation++;
                        }
                        reserve++;
                    }

                    auto& areaReservesDown = data.areaReserves[pays].areaCapacityReservationsDown;
                    reserve = 0;
                    for (const auto& areaReserveDown : areaReservesDown)
                    {
                        uint32_t cluster_participation = 0;
                        for (const auto& clusterReserveParticipation :
                             areaReserveDown.AllSTStorageReservesParticipation)
                        {
                            if (clusterReserveParticipation.maxTurbining > 0)
                            {
                                // 15 (k)
                                STTurbiningMaxReserve.add(pays, reserve, cluster_participation, pdt, false);
                            }
                            if (clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (l)
                                STPumpingMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, false);
                            }
                            if (clusterReserveParticipation.maxTurbining > 0
                                || clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (p)
                                STReserveDownParticipation.add(
                                  pays, reserve, cluster_participation, pdt);
                            }
                            cluster_participation++;
                        }
                        reserve++;
                    }
                }

                 //LongTerm Storage reserve participations
                for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
                {
                    auto& areaReservesUp = data.areaReserves[pays].areaCapacityReservationsUp;
                    uint32_t reserve = 0;
                    for (const auto& areaReserveUp : areaReservesUp)
                    {
                        uint32_t cluster_participation = 0;
                        for (const auto& clusterReserveParticipation :
                             areaReserveUp.AllLTStorageReservesParticipation)
                        {
                            if (clusterReserveParticipation.maxTurbining > 0)
                            {
                                // 15 (a)
                                LTTurbiningMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, true);
                            }
                            if (clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (b)
                                LTPumpingMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, true);
                            }
                            if (clusterReserveParticipation.maxTurbining > 0
                                || clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (e)
                                LTReserveUpParticipation.add(
                                  pays, reserve, cluster_participation, pdt);
                            }
                            cluster_participation++;
                        }
                        reserve++;
                    }

                    auto& areaReservesDown = data.areaReserves[pays].areaCapacityReservationsDown;
                    reserve = 0;
                    for (const auto& areaReserveDown : areaReservesDown)
                    {
                        uint32_t cluster_participation = 0;
                        for (const auto& clusterReserveParticipation :
                             areaReserveDown.AllLTStorageReservesParticipation)
                        {
                            if (clusterReserveParticipation.maxTurbining > 0)
                            {
                                // 15 (a)
                                LTTurbiningMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, false);
                            }
                            if (clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (b)
                                LTPumpingMaxReserve.add(
                                  pays, reserve, cluster_participation, pdt, false);
                            }
                            if (clusterReserveParticipation.maxTurbining > 0
                                || clusterReserveParticipation.maxPumping > 0)
                            {
                                // 15 (f)
                                LTReserveDownParticipation.add(
                                  pays, reserve, cluster_participation, pdt);
                            }
                            cluster_participation++;
                        }
                        reserve++;
                    }
                }
            }
        }
    }
    {
        auto data = GetReserveDataFromProblemHebdo();
        POutCapacityThreasholds pOutCapacityThreasholds(builder_, data);
        POutBounds pOutBounds(builder_, data);
        STTurbiningCapacityThreasholds STTurbiningCapacityThreasholds(builder_, data);
        STPumpingCapacityThreasholds STPumpingCapacityThreasholds(builder_, data);
        LTTurbiningCapacityThreasholds LTTurbiningCapacityThreasholds(builder_, data);
        LTPumpingCapacityThreasholds LTPumpingCapacityThreasholds(builder_, data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                // Thermal Clusters
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays
                  = problemeHebdo_->PaliersThermiquesDuPays[pays];
                for (int cluster = 0; cluster < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                     cluster++)
                {
                    // 17 bis
                    pOutCapacityThreasholds.add(pays, cluster, pdt);

                    // 17 ter
                    pOutBounds.add(pays, cluster, pdt);
                }

                // Short Term Storage Clusters
                const auto& STStorageDuPays = problemeHebdo_->ShortTermStorage[pays];
                for (int cluster = 0; cluster < STStorageDuPays.size(); cluster++)
                {
                    // 15 (m)
                    STTurbiningCapacityThreasholds.add(pays, cluster, pdt);
                    // 15 (n)
                    STPumpingCapacityThreasholds.add(pays, cluster, pdt);
                }
                // Short Term Storage Clusters

                // Long Term Storage Clusters
                const auto& LTStorageDuPays = problemeHebdo_->LongTermStorage[pays];
                for (int cluster = 0; cluster < LTStorageDuPays.size(); cluster++)
                {
                    // 15 (c)
                    LTTurbiningCapacityThreasholds.add(pays, cluster, pdt);
                    // 15 (d)
                    LTPumpingCapacityThreasholds.add(pays, cluster, pdt);
                }
                // Long Term Storage Clusters
            }
        }
    }
}