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


ReserveParticipationGroup::ReserveParticipationGroup(PROBLEME_HEBDO* problemeHebdo,
                                                     bool simulation,
                                                     ConstraintBuilder& builder) :
AbstractStartUpCostsGroup(problemeHebdo,simulation,builder)
{
    this->simulation_ = simulation;
}


ReserveData ReserveParticipationGroup::GetReserveDataFromProblemHebdo()
{
    return {.Simulation = simulation_,
            .areaReserves = problemeHebdo_->allReserves,
            .thermalClusters = problemeHebdo_->PaliersThermiquesDuPays,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim
            };
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

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            // Adding constraints for ReservesUp and ReservesDown
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                auto areaReservesUp
                  = data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp;
                uint32_t reserve = 0;
                for (const auto& areaReserveUp : areaReservesUp)
                {
                    // 24
                    reserveSatisfaction.add(pays, reserve, pdt, true);

                    uint32_t cluster_participation = 0;
                    for (const auto& clusterReserveParticipation :
                         areaReserveUp.AllReservesParticipation)
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
                auto areaReservesDown
                  = data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown;
                for (const auto& areaReserveDown : areaReservesDown)
                {
                    // 24
                    reserveSatisfaction.add(pays, reserve, pdt, false);

                    uint32_t cluster_participation = 0;
                    for (const auto& clusterReserveParticipation :
                         areaReserveDown.AllReservesParticipation)
                    {
                        if (clusterReserveParticipation.maxPower >= 0)
                        {
                            // 16 bis
                            pMaxReserve.add(pays,
                                            reserve, cluster_participation,
                                            pdt,
                                            false);

                            // 17 quater
                            pRunningUnits.add(pays,
                                              reserve, cluster_participation,
                                              pdt,
                                              false);
                        }
                        cluster_participation++;
                    }
                    reserve++;
                }
            }
        }
    }
    {
        auto data = GetReserveDataFromProblemHebdo();
        POutCapacityThreasholds pOutCapacityThreasholds(builder_, data);
        POutBounds pOutBounds(builder_, data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
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
            }
        }
    }
}