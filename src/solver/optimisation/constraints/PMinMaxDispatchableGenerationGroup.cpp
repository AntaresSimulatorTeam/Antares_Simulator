#include "PMinMaxDispatchableGenerationGroup.h"

void PMinMaxDispatchableGenerationGroup::Build()
{
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo_->PaliersThermiquesDuPays[pays];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            PMaxDispatchableGeneration pMaxDispatchableGeneration(builder_);
            PMinDispatchableGeneration pMinDispatchableGeneration(builder_);

            for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                auto data = GetStartUpCostsDataFromProblemHebdo(pays, index, pdt);
                pMaxDispatchableGeneration.add(pays, data);
                pMinDispatchableGeneration.add(pays, data);
            }
        }
    }
}