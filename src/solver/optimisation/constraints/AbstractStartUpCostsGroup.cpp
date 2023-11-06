#include "AbstractStartUpCostsGroup.h"

std::shared_ptr<StartUpCostsData>
  AbstractStartUpCostsGroup::GetStartUpCostsDataFromProblemHebdo(uint32_t pays, int index, int pdt)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays
      = problemeHebdo_->PaliersThermiquesDuPays[pays];
    StartUpCostsData data
      = {PaliersThermiquesDuPays,
         PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index],
         index,
         pdt,
         simulation_};
    return std::make_shared<StartUpCostsData>(data);
}
