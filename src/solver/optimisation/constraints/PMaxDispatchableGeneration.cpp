#include "PMaxDispatchableGeneration.h"

void PMaxDispatchableGeneration::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        double pmaxDUnGroupeDuPalierThermique
          = data.PaliersThermiquesDuPays[pays].PmaxDUnGroupeDuPalierThermique[index];
        int cluster = data.PaliersThermiquesDuPays[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0)
          .NumberOfDispatchableUnits(cluster, -pmaxDUnGroupeDuPalierThermique)
          .lessThan();
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.PMaxDispatchableGeneration(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);
        }
        builder.build();
    }
    else
    {
        *builder.data.NbTermesContraintesPourLesCoutsDeDemarrage += 2;
        builder.data.nombreDeContraintes++;
    }
}
