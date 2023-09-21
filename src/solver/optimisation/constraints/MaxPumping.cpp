#include "MaxPumping.h"

void MaxPumping::add(int pays)
{
    if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
    {
        problemeHebdo->NumeroDeContrainteMaxPompage[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.Pumping(pays, 1.0);
        }
        problemeHebdo->NumeroDeContrainteMaxPompage[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MaxPumping(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.lessThan().build();
    }
    else
        problemeHebdo->NumeroDeContrainteMaxPompage[pays] = -1;
}