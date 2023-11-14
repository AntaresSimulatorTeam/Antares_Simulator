#include "HydroPowerGroup.h"

HydroPowerData HydroPowerGroup::GetHydroPowerDataFromProblemHebdo()
{
    return {
      .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
      .NombreDePasDeTempsPourUneOptimisation = builder_.data.NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteEnergieHydraulique = problemeHebdo_->NumeroDeContrainteEnergieHydraulique};
}

void HydroPowerGroup::Build()
{
    auto hydroPowerData = GetHydroPowerDataFromProblemHebdo();
    HydroPower hydroPower(builder_, hydroPowerData);

    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        hydroPower.add(pays);
    }
}