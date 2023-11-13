#include "HydroPowerGroup.h"

HydroPowerData HydroPowerGroup::GetHydroPowerDataFromProblemHebdo(uint32_t pays)
{
    return {
      .presenceHydro
      = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
      .TurbEntreBornes = problemeHebdo_->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
      .presencePompage
      = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
      .NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteEnergieHydraulique = problemeHebdo_->NumeroDeContrainteEnergieHydraulique,
      .pumpingRatio = problemeHebdo_->CaracteristiquesHydrauliques[pays].PumpingRatio};
}

void HydroPowerGroup::Build()
{
    HydroPower hydroPower(builder_);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        auto hydroPowerData = GetHydroPowerDataFromProblemHebdo(pays);
        hydroPower.add(pays, hydroPowerData);
    }
}