#include "AreaHydroLevelGroup.h"

std::shared_ptr<AreaHydroLevelData> AreaHydroLevelGroup::GetAreaHydroLevelData(int pdt,
                                                                               uint32_t pays)
{
    AreaHydroLevelData data
      = {problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire,
         problemeHebdo_->CaracteristiquesHydrauliques[pays].PumpingRatio};
    return std::make_shared<AreaHydroLevelData>(data);
}

void AreaHydroLevelGroup::Build()
{
    AreaHydroLevel areaHydroLevel(builder_);

    for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            areaHydroLevel.add(pays, pdt, GetAreaHydroLevelData(pdt, pays));
        }
    }
}