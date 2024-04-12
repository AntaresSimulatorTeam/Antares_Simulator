#include "antares/solver/optimisation/constraints/PowerOutputVariation.h"

void PowerOutputVariation::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        int cluster = data.PaliersThermiquesDuPays[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        double pmaxDUnGroupeDuPalierThermique
          = data.PaliersThermiquesDuPays[pays].PmaxDUnGroupeDuPalierThermique[index];
        // constraint : P(t) - P(t-1) - u * M^+(t) - P^+ + P^- + u * M^-(t) = 0
        builder.updateHourWithinWeek(pdt)
            .DispatchableProduction(cluster, 1.0)
            .DispatchableProduction(cluster, -1.0, -1, builder.data.NombreDePasDeTempsPourUneOptimisation)
          .NumberStartingDispatchableUnits(cluster, -pmaxDUnGroupeDuPalierThermique)
            .ProductionIncreaseAboveMin(cluster, -1.0)
          .ProductionDecreaseAboveMin(cluster, 1.0)
          .NumberStoppingDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
            .equalTo();
   
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
        builder.data.NbTermesContraintesPourLesRampes += 5;
        builder.data.nombreDeContraintes++;
    }
}
