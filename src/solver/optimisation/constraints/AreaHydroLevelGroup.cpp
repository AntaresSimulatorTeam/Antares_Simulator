#include "AreaHydroLevelGroup.h"

AreaHydroLevelData AreaHydroLevelGroup::GetAreaHydroLevelData(int pdt, uint32_t pays)
{
    return {
      .NumeroDeContrainteDesNiveauxPays
      = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays,
      .SuiviNiveauHoraire = problemeHebdo_->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire,
      .PumpingRatio = problemeHebdo_->CaracteristiquesHydrauliques[pays].PumpingRatio};
}

void AreaHydroLevelGroup::Build()
{
    AreaHydroLevel areaHydroLevel(builder_);

    for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            auto areaHydroLevelData = GetAreaHydroLevelData(pdt, pays);
            areaHydroLevel.add(pays, pdt, areaHydroLevelData);
        }
    }
}