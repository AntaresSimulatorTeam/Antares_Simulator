#include "antares/solver/optimisation/constraints/HydraulicSmoothingGroup.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationSum.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxDown.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxUp.h"

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
                continue;

            hydroPowerSmoothingUsingVariationSum.add(
              pays, problemeHebdo_->NombreDePasDeTempsPourUneOptimisation);
        }
    }

    else if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, pdt);
            }
        }
    }
}