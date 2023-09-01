#include "MaxPumping.h"

void MaxPumping::add(int pays)
{
    if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
    {
        problemeHebdo->NumeroDeContrainteMaxPompage[pays] = ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::Pumping(pays), 1.0);
        }

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MaxPumping(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.lessThan().build();
    }
}