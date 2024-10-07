#include "AbstractStartUpCostsGroup.h"

AbstractStartUpCostsGroup::AbstractStartUpCostsGroup(PROBLEME_HEBDO* problemeHebdo,
                                                     bool simulation,
                                                     ConstraintBuilder& builder) :
  ConstraintGroup(problemeHebdo, builder), simulation_(simulation)
{
}


StartUpCostsData AbstractStartUpCostsGroup::GetStartUpCostsDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_};
}
