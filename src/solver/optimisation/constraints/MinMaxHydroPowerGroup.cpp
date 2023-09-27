#include "MinMaxHydroPowerGroup.h"
std::shared_ptr<MinHydroPowerData> MinMaxHydroPowerGroup::GetMinHydroPowerData(

  uint32_t pays)
{
    MinHydroPowerData data
      = {problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
         problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
         problemeHebdo_->NumeroDeContrainteMinEnergieHydraulique};

    return std::make_shared<MinHydroPowerData>(data);
}

std::shared_ptr<MaxHydroPowerData> MinMaxHydroPowerGroup::GetMaxHydroPowerData(

  uint32_t pays)
{
    MaxHydroPowerData data
      = {problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
         problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
         problemeHebdo_->NumeroDeContrainteMaxEnergieHydraulique};

    return std::make_shared<MaxHydroPowerData>(data);
}

void MinMaxHydroPowerGroup::Build()
{
    MinHydroPower minHydroPower(builder_);
    MaxHydroPower maxHydroPower(builder_);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        minHydroPower.add(pays, GetMinHydroPowerData(pays));

        maxHydroPower.add(pays, GetMaxHydroPowerData(pays));
    }
}