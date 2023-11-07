#include "NbUnitsOutageLessThanNbUnitsStopGroup.h"

std::shared_ptr<NbUnitsOutageLessThanNbUnitsStopData>
  NbUnitsOutageLessThanNbUnitsStopGroup::GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo(
    uint32_t pays,
    int index,
    int pdt)
{
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt];
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo_->PaliersThermiquesDuPays[pays];
    NbUnitsOutageLessThanNbUnitsStopData data
      = {PaliersThermiquesDuPays,
         PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index],
         index,
         pdt,
         simulation_,
         CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesDeDureeMinDeMarche};
    return std::make_shared<NbUnitsOutageLessThanNbUnitsStopData>(data);
}

/**
 * @brief build NbUnitsOutageLessThanNbUnitsStopGroup constraints with
 * respect to default order
 */
void NbUnitsOutageLessThanNbUnitsStopGroup::Build()
{
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo_->PaliersThermiquesDuPays[pays];
        NbUnitsOutageLessThanNbUnitsStop nbUnitsOutageLessThanNbUnitsStop(builder_);

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbUnitsOutageLessThanNbUnitsStop.add(
                  pays, GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo(pays, index, pdt));
            }
        }
    }
}