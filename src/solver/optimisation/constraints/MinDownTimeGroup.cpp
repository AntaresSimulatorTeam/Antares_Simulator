#include "MinDownTimeGroup.h"

MinDownTimeData MinDownTimeGroup::GetMinDownTimeDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build MinDownTime constraints with
 * respect to default order
 */
void MinDownTimeGroup::Build()
{
    auto data = GetMinDownTimeDataFromProblemHebdo();
    MinDownTime minDownTime(builder_, data);
    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                minDownTime.add(pays, index, pdt);
            }
        }
    }
}