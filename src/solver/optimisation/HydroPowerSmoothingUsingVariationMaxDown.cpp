#include "HydroPowerSmoothingUsingVariationMaxDown.h"

void HydroPowerSmoothingUsingVariationMaxDown::add(int pays, int pdt)
{
    if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
    {
        return;
    }

    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                          problemeHebdo->NamedProblems);
    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
    namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxDown(
      problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .HydProd(pays, 1.0)
      .updateHourWithinWeek(0)
      .HydProdDown(pays, -1.0)
      .lessThan()
      .build();
}