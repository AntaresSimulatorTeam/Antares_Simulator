#include "MaxPumpingGroup.h"

MaxPumpingData MaxPumpingGroup::GetMaxPumpingData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteMaxPompage = problemeHebdo_->NumeroDeContrainteMaxPompage};
}

void MaxPumpingGroup::Build()
{
    auto maxPumpingData = GetMaxPumpingData();
    MaxPumping maxPumping(builder_, maxPumpingData);

    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        maxPumping.add(pays);
    }
}