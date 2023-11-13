#include "MinMaxHydroPowerGroup.h"
MinHydroPowerData MinMaxHydroPowerGroup::GetMinHydroPowerData(

  uint32_t pays)
{
    return {
      .presenceHydro
      = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
      .TurbEntreBornes = problemeHebdo_->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
      .PresenceDePompageModulable
      = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
      .NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteMinEnergieHydraulique
      = problemeHebdo_->NumeroDeContrainteMinEnergieHydraulique};
}

MaxHydroPowerData MinMaxHydroPowerGroup::GetMaxHydroPowerData(

  uint32_t pays)
{
    return {
      .presenceHydro
      = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
      .TurbEntreBornes = problemeHebdo_->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
      .PresenceDePompageModulable
      = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
      .NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteMaxEnergieHydraulique
      = problemeHebdo_->NumeroDeContrainteMaxEnergieHydraulique};
}

void MinMaxHydroPowerGroup::Build()
{
    MinHydroPower minHydroPower(builder_);
    MaxHydroPower maxHydroPower(builder_);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        auto minHydroPowerData = GetMinHydroPowerData(pays);
        minHydroPower.add(pays, minHydroPowerData);
        auto maxHydroPowerData = GetMaxHydroPowerData(pays);
        maxHydroPower.add(pays, maxHydroPowerData);
    }
}