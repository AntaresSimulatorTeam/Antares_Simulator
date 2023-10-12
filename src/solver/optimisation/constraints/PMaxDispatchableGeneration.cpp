#include "PMaxDispatchableGeneration.h"

void PMaxDispatchableGeneration::add(int pays,
                                     int cluster,
                                     int clusterIndex,
                                     int pdt,
                                     bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays
          = problemeHebdo->PaliersThermiquesDuPays[pays];
        double pmaxDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.PmaxDUnGroupeDuPalierThermique[clusterIndex];

        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0)
          .NumberOfDispatchableUnits(cluster, -pmaxDUnGroupeDuPalierThermique)
          .lessThan();
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.PMaxDispatchableGeneration(
              problemeHebdo->ProblemeAResoudre->NombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
        }
        builder.build();
    }
    else
    {
        problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage += 2;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
