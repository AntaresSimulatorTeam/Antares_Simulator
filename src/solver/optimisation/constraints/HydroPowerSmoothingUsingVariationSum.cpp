#include "HydroPowerSmoothingUsingVariationSum.h"

void HydroPowerSmoothingUsingVariationSum::add(int pays)
{
    if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
    {
        return;
    }

    const int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int pdt1 = pdt + 1;
        if (pdt1 >= nombreDePasDeTempsPourUneOptimisation)
            pdt1 = 0;
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.HydroPowerSmoothingUsingVariationSum(
          problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .HydProd(pays, 1.0)
          .updateHourWithinWeek(pdt1)
          .HydProd(pays, -1.0)
          .updateHourWithinWeek(pdt)
          .HydProdDown(pays, -1.0)
          .HydProdUp(pays, 1.0)
          .equalTo()
          .build();
    }
}