#include "NbDispUnitsMinBoundSinceMinUpTimeGroup.h"

NbDispUnitsMinBoundSinceMinUpTimeData
  NbDispUnitsMinBoundSinceMinUpTimeGroup::GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build NbUnitsOutageLessThanNbUnitsStopGroup constraints with
 * respect to default order
 */
void NbDispUnitsMinBoundSinceMinUpTimeGroup::Build()
{
    auto data = GetNbDispUnitsMinBoundSinceMinUpTimeDataFromProblemHebdo();
    NbDispUnitsMinBoundSinceMinUpTime nbDispUnitsMinBoundSinceMinUpTime(builder_, data);
    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            if (PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index] <= 0)
                continue;

            for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                nbDispUnitsMinBoundSinceMinUpTime.add(pays, index, pdt);
            }
        }
    }
}