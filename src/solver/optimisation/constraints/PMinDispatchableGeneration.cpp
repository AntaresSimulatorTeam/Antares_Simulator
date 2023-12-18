#include "PMinDispatchableGeneration.h"

void PMinDispatchableGeneration::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        double pminDUnGroupeDuPalierThermique
          = data.PaliersThermiquesDuPays[pays].pminDUnGroupeDuPalierThermique[index];

        int cluster = data.PaliersThermiquesDuPays[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0)
          .NumberOfDispatchableUnits(cluster, -pminDUnGroupeDuPalierThermique)
          .greaterThan();
        /*consider Adding naming constraint inside the builder*/
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.PMinDispatchableGeneration(
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
