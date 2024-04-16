#include "antares/solver/optimisation/constraints/PowerOutputVariationIncrease.h"

void PowerOutputVariationIncrease::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        int cluster = data.PaliersThermiquesDuPays[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        double pmaxDUnGroupeDuPalierThermique
          = data.PaliersThermiquesDuPays[pays].PmaxDUnGroupeDuPalierThermique[index];
        // constraint : P(t) - P(t-1) - u * M^+(t) - P^+ <= 0
        builder.updateHourWithinWeek(pdt)
            .DispatchableProduction(cluster, 1.0)
            .DispatchableProduction(cluster, -1.0, -1, builder.data.NombreDePasDeTempsPourUneOptimisation)
          .NumberStartingDispatchableUnits(cluster, -pmaxDUnGroupeDuPalierThermique)
            .ProductionIncreaseAboveMin(cluster, -1.0)
            .lessThan();
   
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.ProductionOutputVariation(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);
        }
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesRampes += 4;
        builder.data.nombreDeContraintes++;
    }
}
