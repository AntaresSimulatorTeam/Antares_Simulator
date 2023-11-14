#include "PMinMaxDispatchableGenerationGroup.h"

/**
 * @brief build P{min,max}DispatchableGeneration constraints with
 * respect to default order
 */
void PMinMaxDispatchableGenerationGroup::Build()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    PMaxDispatchableGeneration pMaxDispatchableGeneration(builder_, data);
    PMinDispatchableGeneration pMinDispatchableGeneration(builder_, data);
    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                pMaxDispatchableGeneration.add(pays, index, pdt);
                pMinDispatchableGeneration.add(pays, index, pdt);
            }
        }
    }
}