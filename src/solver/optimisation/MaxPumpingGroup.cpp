#include "MaxPumpingGroup.h"

std::shared_ptr<MaxPumpingData> MaxPumpingGroup::GetMaxPumpingData(uint32_t pays)
{
    MaxPumpingData data
      = {problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
         problemeHebdo_->NumeroDeContrainteMaxPompage};

    return std::make_shared<MaxPumpingData>(data);
}

void MaxPumpingGroup::Build()
{
    MaxPumping maxPumping(builder_);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        maxPumping.add(pays, GetMaxPumpingData(pays));
    }
}