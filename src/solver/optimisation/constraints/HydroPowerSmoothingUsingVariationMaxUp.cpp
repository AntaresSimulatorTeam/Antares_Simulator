#include "HydroPowerSmoothingUsingVariationMaxUp.h"

void HydroPowerSmoothingUsingVariationMaxUp::add(int pays, int pdt)
{
    if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
    {
        return;
    }

    ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                          problemeHebdo->NamedProblems);
    namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
    namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxUp(
      problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .HydProd(pays, 1.0)
      .updateHourWithinWeek(0)
      .HydProdUp(pays, -1.0)
      .greaterThan()
      .build();
}