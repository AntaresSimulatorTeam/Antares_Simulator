#include "HydroPowerGroup.h"
#include "new_constraint_builder_utils.h"

std::shared_ptr<HydroPowerData> HydroPowerGroup::GetHydroPowerDataFromProblemHebdo(uint32_t pays)
{
    HydroPowerData data
      = {problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
         problemeHebdo_->NombreDePasDeTempsPourUneOptimisation,
         problemeHebdo_->NumeroDeContrainteEnergieHydraulique,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].PumpingRatio};
    return std::make_shared<HydroPowerData>(data);
}

void HydroPowerGroup::Build()
{
    std::shared_ptr<NewConstraintBuilder> builder(
      NewGetConstraintBuilderFromProblemHebdo(problemeHebdo_));
    HydroPower hydroPower(builder);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        hydroPower.add(pays, GetHydroPowerDataFromProblemHebdo(pays));
    }
}