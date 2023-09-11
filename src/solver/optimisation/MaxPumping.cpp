#include "MaxPumping.h"

void MaxPumping::add(int pays, MaxPumpingData& data)
{
    if (data.PresenceDePompageModulable)
    {
        data.NumeroDeContrainteMaxPompage[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        const int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.include(Variable::Pumping(pays), 1.0);
        }
        data.NumeroDeContrainteMaxPompage[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.MaxPumping(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxPompage[pays] = -1;
}