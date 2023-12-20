#include "PowerOutputVariation.h"

void PowerOutputVariation::add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[pays];
        int rampingClusterIndex
          = PaliersThermiquesDuPays.clusterRampingVariablesIndex[clusterIndex];
        double maxUpwardPowerRampingRate
          = PaliersThermiquesDuPays.maxUpwardPowerRampingRate[rampingClusterIndex];
        double pminDUnGroupeDuPalierThermique = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];
        // constraint : P(t) - P(t-1) - l * M^+(t) - P^+ + P^- = 0
        builder.updateHourWithinWeek(pdt)
            .DispatchableProduction(cluster, 1.0)
            .DispatchableProduction(cluster, -1.0, -1, problemeHebdo->NombreDePasDeTempsPourUneOptimisation)
            .NumberStartingDispatchableUnits(cluster, -pminDUnGroupeDuPalierThermique)
            .ProductionIncreaseAboveMin(cluster, -1.0)
            .ProductionDecreaseAboveMin(cluster, 1.0)
            .equalTo();
   
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);

            namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
            namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);

            namer.ProductionOutputVariation(problemeHebdo->ProblemeAResoudre->NombreDeContraintes, PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
        }
        builder.build();
    }
    else
    {
        problemeHebdo->NbTermesContraintesPourLesRampes += 5;
        problemeHebdo->ProblemeAResoudre->NombreDeContraintes++;
    }
}
