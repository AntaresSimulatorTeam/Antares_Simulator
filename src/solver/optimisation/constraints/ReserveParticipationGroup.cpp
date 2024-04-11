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


PMaxReserveData ReserveParticipationGroup::GetPMaxReserveDataFromProblemHebdo()
{
    return {.Simulation = simulation_, .areaReserves = problemeHebdo_->allReserves};
}

/**
 * @brief build MinDownTime constraints with
 * respect to default order
 */
void ReserveParticipationGroup::BuildConstraints()
{
    auto data = GetPMaxReserveDataFromProblemHebdo();
    PMaxReserve pMaxReserve(builder_, data);

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
                uint32_t cluster = 0;
                for (const auto& clusterReserveParticipation :
                     areaReserveUp.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        pMaxReserve.add(pays, reserve, cluster, pdt, true);
                    }
                    cluster++;
                }
                reserve++;
            }

            auto areaReservesDown
              = data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown;
            reserve = 0;
            for (const auto& areaReserveDown : areaReservesDown)
            {
                uint32_t cluster = 0;
                for (const auto& clusterReserveParticipation :
                     areaReserveDown.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        pMaxReserve.add(pays, reserve, cluster, pdt, false);
                    }
                    cluster++;
                }
                reserve++;
            }
        }
    }
}