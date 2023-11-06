#include "PMinMaxDispatchableGenerationGroup.h"

std::shared_ptr<PMinDispatchableGenerationData>
  PMinMaxDispatchableGenerationGroup::GetPMinDispatchableGenerationDataFromProblemHebdo(
    uint32_t pays,
    int index,
    int pdt)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo_->PaliersThermiquesDuPays[pays];
    PMinDispatchableGenerationData data
      = {PaliersThermiquesDuPays,
         PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index],
         index,
         pdt,
         simulation_};
    return std::make_shared<PMinDispatchableGenerationData>(data);
}

std::shared_ptr<PMaxDispatchableGenerationData>
  PMinMaxDispatchableGenerationGroup::GetPMaxDispatchableGenerationDataFromProblemHebdo(
    uint32_t pays,
    int index,
    int pdt)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo_->PaliersThermiquesDuPays[pays];
    PMaxDispatchableGenerationData data
      = {PaliersThermiquesDuPays,
         PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index],
         index,
         pdt,
         simulation_};
    return std::make_shared<PMaxDispatchableGenerationData>(data);
}

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
                pMaxDispatchableGeneration.add(
                  pays, GetPMaxDispatchableGenerationDataFromProblemHebdo(pays, index, pdt));
                pMinDispatchableGeneration.add(
                  pays, GetPMinDispatchableGenerationDataFromProblemHebdo(pays, index, pdt));
            }
        }
    }
}