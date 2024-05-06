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

#include "antares/solver/optimisation/constraints/NbDispUnitsMinBoundSinceMinUpTimeGroup.h"

NbDispUnitsMinBoundSinceMinUpTimeData
NbDispUnitsMinBoundSinceMinUpTimeGroup::GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build NbUnitsOutageLessThanNbUnitsStopGroup constraints with
 * respect to default order
 */
void NbDispUnitsMinBoundSinceMinUpTimeGroup::BuildConstraints()
{
    auto data = GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo();
    NbDispUnitsMinBoundSinceMinUpTime nbDispUnitsMinBoundSinceMinUpTime(builder_, data);
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            if (PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index] <= 0)
            {
                continue;
            }

            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbDispUnitsMinBoundSinceMinUpTime.add(pays, index, pdt);
            }
        }
    }
}
