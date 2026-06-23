// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/PowerOutputVariationGroup.h"

/**
 * @brief build P{min,max}DispatchableGeneration constraints with
 * respect to default order
 */
void PowerOutputVariationGroup::BuildConstraints()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    PMaxDispatchableGeneration pMaxDispatchableGeneration(builder_, data);
    PMinDispatchableGeneration pMinDispatchableGeneration(builder_, data);
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] >= 0)
            {
                PowerOutputVariationIncrease powerOutputVariationIncrease(builder_, data);
                PowerOutputVariationDecrease powerOutputVariationDecrease(builder_, data);

                for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
                     pdt++)
                {
                    powerOutputVariationIncrease.add(pays, index, pdt);
                    powerOutputVariationDecrease.add(pays, index, pdt);
                }
            }
        }
    }
}
