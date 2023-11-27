#include "ConsistenceNumberOfDispatchableUnits.h"

void ConsistenceNumberOfDispatchableUnits::add(int pays,
                                               int cluster,
                                               int clusterIndex,
                                               int pdt,
                                               bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];

        int NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

        int Pdtmoins1 = pdt - 1;
        if (Pdtmoins1 < 0)
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;

        builder.updateHourWithinWeek(pdt)
          .NumberOfDispatchableUnits(cluster, 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .NumberOfDispatchableUnits(cluster, -1)
          .updateHourWithinWeek(pdt)
          .NumberStartingDispatchableUnits(cluster, -1)
          .NumberStoppingDispatchableUnits(cluster, 1)
          .equalTo();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.ConsistenceNODU(problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
                                  PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);

            builder.build();
        }
    }
    else
    {
        problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage += 4;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
