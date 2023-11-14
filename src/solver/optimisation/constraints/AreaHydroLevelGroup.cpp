#include "AreaHydroLevelGroup.h"

AreaHydroLevelData AreaHydroLevelGroup::GetAreaHydroLevelData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques};
}

void AreaHydroLevelGroup::Build()
{
    auto areaHydroLevelData = GetAreaHydroLevelData();
    AreaHydroLevel areaHydroLevel(builder_, areaHydroLevelData);

    for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
        {
            areaHydroLevel.add(pays, pdt);
        }
    }
}