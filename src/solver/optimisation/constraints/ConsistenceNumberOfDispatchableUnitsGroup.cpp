#include "ConsistenceNumberOfDispatchableUnitsGroup.h"
#include "ConsistenceNumberOfDispatchableUnits.h"

/**
 * @brief build ConsistenceNumberOfDispatchableUnits constraints with
 * respect to default order
 */
void ConsistenceNumberOfDispatchableUnitsGroup::BuildConstraints()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    ConsistenceNumberOfDispatchableUnits consistenceNumberOfDispatchableUnits(builder_, data);
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo_->PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                consistenceNumberOfDispatchableUnits.add(pays, index, pdt);
            }
        }
    }
}