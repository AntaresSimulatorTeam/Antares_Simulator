#include "MaxPumpingGroup.h"

MaxPumpingData MaxPumpingGroup::GetMaxPumpingData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteMaxPompage = problemeHebdo_->NumeroDeContrainteMaxPompage};
}

void MaxPumpingGroup::BuildConstraints()
{
    auto maxPumpingData = GetMaxPumpingData();
    MaxPumping maxPumping(builder_, maxPumpingData);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        maxPumping.add(pays);
    }
}