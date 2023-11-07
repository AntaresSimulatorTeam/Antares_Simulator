#include "MinDownTimeGroup.h"

std::shared_ptr<MinDownTimeData> MinDownTimeGroup::GetMinDownTimeDataFromProblemHebdo(uint32_t pays,
                                                                                      int index,
                                                                                      int pdt)
{
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt];
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo_->PaliersThermiquesDuPays[pays];
    MinDownTimeData data
      = {PaliersThermiquesDuPays,
         PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index],
         index,
         pdt,
         simulation_,
         CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesDeDureeMinDArret};
    return std::make_shared<MinDownTimeData>(data);
}
void MinDownTimeGroup::Build()
{
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo_->PaliersThermiquesDuPays[pays];
        MinDownTime minDownTime(builder_);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier
              = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                minDownTime.add(pays, GetMinDownTimeDataFromProblemHebdo(pays, index, pdt));
            }
        }
    }
}