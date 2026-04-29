// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/NbUnitsOutageLessThanNbUnitsStopGroup.h"

NbUnitsOutageLessThanNbUnitsStopData
NbUnitsOutageLessThanNbUnitsStopGroup::GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build NbUnitsOutageLessThanNbUnitsStopGroup constraints with
 * respect to default order
 */
void NbUnitsOutageLessThanNbUnitsStopGroup::BuildConstraints()
{
    auto data = GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo();
    NbUnitsOutageLessThanNbUnitsStop nbUnitsOutageLessThanNbUnitsStop(builder_, data);
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbUnitsOutageLessThanNbUnitsStop.add(pays, index, pdt);
            }
        }
    }
}
