#include "NbDispUnitsMinBoundSinceMinUpTimeGroup.h"

std::shared_ptr<NbDispUnitsMinBoundSinceMinUpTimeData>
  NbDispUnitsMinBoundSinceMinUpTimeGroup::GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo(
    uint32_t pays,
    int index,
    int pdt)
{
    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt];
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo_->PaliersThermiquesDuPays[pays];
    NbDispUnitsMinBoundSinceMinUpTimeData data
      = {PaliersThermiquesDuPays,
         PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index],
         index,
         pdt,
         simulation_,
         CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesDeDureeMinDeMarche};
    return std::make_shared<NbDispUnitsMinBoundSinceMinUpTimeData>(data);
}
void NbDispUnitsMinBoundSinceMinUpTimeGroup::Build()
{
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        NbDispUnitsMinBoundSinceMinUpTime nbDispUnitsMinBoundSinceMinUpTime(builder_);
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            if (PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index] <= 0)
                continue;

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbDispUnitsMinBoundSinceMinUpTime.add(
                  pays, GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo(pays, index, pdt));
            }
        }
    }
}