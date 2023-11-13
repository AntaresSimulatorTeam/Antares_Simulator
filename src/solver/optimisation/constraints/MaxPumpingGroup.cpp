#include "MaxPumpingGroup.h"

MaxPumpingData MaxPumpingGroup::GetMaxPumpingData(uint32_t pays)
{
    return {.PresenceDePompageModulable
            = problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
            .NumeroDeContrainteMaxPompage = problemeHebdo_->NumeroDeContrainteMaxPompage};
}

void MaxPumpingGroup::Build()
{
    MaxPumping maxPumping(builder_);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        auto maxPumpingData = GetMaxPumpingData(pays);
        maxPumping.add(pays, maxPumpingData);
    }
}