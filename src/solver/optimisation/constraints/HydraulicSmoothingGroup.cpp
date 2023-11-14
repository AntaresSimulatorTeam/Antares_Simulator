#include "HydraulicSmoothingGroup.h"
#include "HydroPowerSmoothingUsingVariationSum.h"
#include "HydroPowerSmoothingUsingVariationMaxDown.h"
#include "HydroPowerSmoothingUsingVariationMaxUp.h"

void HydraulicSmoothingGroup::Build()
{
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(builder_);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(builder_);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(builder_);

    if (typeDeLissageHydraulique_ == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
        {
            if (!builder_.data.CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            hydroPowerSmoothingUsingVariationSum.add(
              pays, builder_.data.NombreDePasDeTempsPourUneOptimisation);
        }
    }

    else if (typeDeLissageHydraulique_ == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
        {
            if (!builder_.data.CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, pdt);
            }
        }
    }
}