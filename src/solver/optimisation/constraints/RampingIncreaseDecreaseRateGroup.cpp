// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/RampingIncreaseDecreaseRateGroup.h"

/**
 * @brief build P{min,max}DispatchableGeneration constraints with
 * respect to default order
 */
void RampingIncreaseDecreaseRateGroup::BuildConstraints()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            if (PaliersThermiquesDuPays.maxUpwardPowerRampingRate[index] >= 0)
            {
                RampingIncreaseRate rampingIncreaseRate(builder_, data);
                RampingDecreaseRate rampingDecreaseRate(builder_, data);

                for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;
                     pdt++)
                {
                    rampingIncreaseRate.add(pays, index, pdt);
                    rampingDecreaseRate.add(pays, index, pdt);
                }
            }
        }
    }
}
