#include "HydroPowerGroup.h"

HydroPowerData HydroPowerGroup::GetHydroPowerDataFromProblemHebdo()
{
    return {
      .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
      .NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteEnergieHydraulique = problemeHebdo_->NumeroDeContrainteEnergieHydraulique};
}

void HydroPowerGroup::BuildConstraints()
{
    auto hydroPowerData = GetHydroPowerDataFromProblemHebdo();
    HydroPower hydroPower(builder_, hydroPowerData);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        hydroPower.add(pays);
    }
}