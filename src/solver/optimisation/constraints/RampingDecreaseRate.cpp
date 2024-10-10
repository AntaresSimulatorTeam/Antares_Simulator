#include "antares/solver/optimisation/constraints/RampingDecreaseRate.h"

void RampingDecreaseRate::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        int cluster = data.PaliersThermiquesDuPays[pays]
                        .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        double maxDownwardPowerRampingRate
          = data.PaliersThermiquesDuPays[pays].maxDownwardPowerRampingRate[index];
        double pmaxDUnGroupeDuPalierThermique
          = data.PaliersThermiquesDuPays[pays].PmaxDUnGroupeDuPalierThermique[index];
        // constraint : P(t) - P(t-1) + R^- * M(t) + u * M^-(t) + u * M^--(t) > 0
        builder.updateHourWithinWeek(pdt)
            .DispatchableProduction(cluster, 1.0)
            .DispatchableProduction(cluster, -1.0, -1, builder.data.NombreDePasDeTempsPourUneOptimisation)
            .NumberOfDispatchableUnits(cluster, maxDownwardPowerRampingRate)
            .NumberStoppingDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
            .greaterThan();
 
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.RampingDecreaseRate(
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
