#include "RampingDecreaseRate.h"

void RampingDecreaseRate::add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
        double maxDownwardPowerRampingRate = PaliersThermiquesDuPays.maxDownwardPowerRampingRate[clusterIndex];
        double pmaxDUnGroupeDuPalierThermique = PaliersThermiquesDuPays.PmaxDUnGroupeDuPalierThermique[clusterIndex];
        // constraint : P(t) - P(t-1) + R^- * M(t) + u * M^-(t) + u * M^--(t) > 0
        if (pdt > 0)
        {
            builder.updateHourWithinWeek(pdt)
              .DispatchableProduction(cluster, 1.0)
              .DispatchableProduction(cluster, -1.0, -1, problemeHebdo->NombreDePasDeTempsPourUneOptimisation)
              .NumberOfDispatchableUnits(cluster, maxDownwardPowerRampingRate)
              .NumberStoppingDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
              .NumberBreakingDownDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
              .greaterThan();
        }
        else
        {
            builder.updateHourWithinWeek(pdt)
              .DispatchableProduction(cluster, 1.0)
              .NumberOfDispatchableUnits(cluster, maxDownwardPowerRampingRate)
              .NumberStoppingDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
              .NumberBreakingDownDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
              .greaterThan();
        }
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.RampingDecreaseRate(problemeHebdo->ProblemeAResoudre->NombreDeContraintes, 
                PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
        }
        builder.build();
    }
    else
    {
        int add = (pdt == 0) ? 4 : 5;
        problemeHebdo->NbTermesContraintesPourLesRampes += add;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
