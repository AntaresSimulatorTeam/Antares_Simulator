#include "ConsistenceNumberOfDispatchableUnitsGroup.h"
#include "ConsistenceNumberOfDispatchableUnits.h"

/**
 * @brief build ConsistenceNumberOfDispatchableUnits constraints with
 * respect to default order
 */
void ConsistenceNumberOfDispatchableUnitsGroup::Build()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    ConsistenceNumberOfDispatchableUnits consistenceNumberOfDispatchableUnits(builder_, data);
    for (uint32_t pays = 0; pays < builder_.data.NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < builder_.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                consistenceNumberOfDispatchableUnits.add(pays, index, pdt);
            }
        }
    }
}