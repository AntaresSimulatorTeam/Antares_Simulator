#include "RampingIncreaseRate.h"

void RampingIncreaseRate::add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
        double maxUpwardPowerRampingRate = PaliersThermiquesDuPays.maxUpwardPowerRampingRate[clusterIndex];
        double pminDUnGroupeDuPalierThermique = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];
        // constraint : P(t) - P(t-1) - R^+ * M(t) - l * M^+(t) < 0

        builder.updateHourWithinWeek(pdt)
            .DispatchableProduction(cluster, 1.0)
            .DispatchableProduction(
            cluster, -1.0, -1, problemeHebdo->NombreDePasDeTempsPourUneOptimisation)
            .NumberOfDispatchableUnits(cluster, -maxUpwardPowerRampingRate)
            .NumberStartingDispatchableUnits(cluster, -pminDUnGroupeDuPalierThermique)
            .lessThan();
        
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            namer.RampingIncreaseRate(problemeHebdo->ProblemeAResoudre->NombreDeContraintes, 
                PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
        }
        builder.build();
    }
    else
    {
        problemeHebdo->NbTermesContraintesPourLesRampes += 4;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
