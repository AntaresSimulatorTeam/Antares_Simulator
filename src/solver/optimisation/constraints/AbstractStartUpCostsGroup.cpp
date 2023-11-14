#include "AbstractStartUpCostsGroup.h"

StartUpCostsData AbstractStartUpCostsGroup::GetStartUpCostsDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_};
}
