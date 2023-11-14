#include "NbUnitsOutageLessThanNbUnitsStopGroup.h"

NbUnitsOutageLessThanNbUnitsStopData
  NbUnitsOutageLessThanNbUnitsStopGroup::GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build NbUnitsOutageLessThanNbUnitsStopGroup constraints with
 * respect to default order
 */
void NbUnitsOutageLessThanNbUnitsStopGroup::Build()
{
    auto data = GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo();
    NbUnitsOutageLessThanNbUnitsStop nbUnitsOutageLessThanNbUnitsStop(builder_, data);
    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo_->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbUnitsOutageLessThanNbUnitsStop.add(pays, index, pdt);
            }
        }
    }
}