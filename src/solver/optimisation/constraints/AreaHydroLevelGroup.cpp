#include "AreaHydroLevelGroup.h"

AreaHydroLevelData AreaHydroLevelGroup::GetAreaHydroLevelData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques};
}

void AreaHydroLevelGroup::BuildConstraints()
{
    auto areaHydroLevelData = GetAreaHydroLevelData();
    AreaHydroLevel areaHydroLevel(builder_, areaHydroLevelData);

    for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            areaHydroLevel.add(pays, pdt);
        }
    }
}