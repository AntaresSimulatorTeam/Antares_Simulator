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

#include "antares/solver/optimisation/constraints/HydraulicSmoothingGroup.h"

#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxDown.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxUp.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationSum.h"

void HydraulicSmoothingGroup::BuildConstraints()
{
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(builder_);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(builder_);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(builder_);

    if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                continue;
            }

            hydroPowerSmoothingUsingVariationSum
              .add(pays, problemeHebdo_->NombreDePasDeTempsPourUneOptimisation);
        }
    }

    else if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                continue;
            }

            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, pdt);
            }
        }
    }
}
