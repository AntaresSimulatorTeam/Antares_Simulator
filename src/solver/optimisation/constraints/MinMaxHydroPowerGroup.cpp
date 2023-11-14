#include "MinMaxHydroPowerGroup.h"
MinHydroPowerData MinMaxHydroPowerGroup::GetMinHydroPowerData()
{
    return {
      .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
      .NombreDePasDeTempsPourUneOptimisation = builder_.data.NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteMinEnergieHydraulique
      = problemeHebdo_->NumeroDeContrainteMinEnergieHydraulique};
}

MaxHydroPowerData MinMaxHydroPowerGroup::GetMaxHydroPowerData()
{
    return {
      .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
      .NombreDePasDeTempsPourUneOptimisation = builder_.data.NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteMaxEnergieHydraulique
      = problemeHebdo_->NumeroDeContrainteMaxEnergieHydraulique};
}

void MinMaxHydroPowerGroup::Build()
{
    auto minHydroPowerData = GetMinHydroPowerData();
    MinHydroPower minHydroPower(builder_, minHydroPowerData);
    auto maxHydroPowerData = GetMaxHydroPowerData();
    MaxHydroPower maxHydroPower(builder_, maxHydroPowerData);

    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        minHydroPower.add(pays);
        maxHydroPower.add(pays);
    }
}