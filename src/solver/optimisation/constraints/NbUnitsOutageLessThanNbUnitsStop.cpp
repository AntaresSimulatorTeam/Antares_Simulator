#include "NbUnitsOutageLessThanNbUnitsStop.h"

void NbUnitsOutageLessThanNbUnitsStop::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        auto cluster = data.PaliersThermiquesDuPays[pays]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
          = -1;

        builder.updateHourWithinWeek(pdt)
          .NumberBreakingDownDispatchableUnits(cluster, 1.0)
          .NumberStoppingDispatchableUnits(cluster, -1.0)
          .lessThan();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.NbUnitsOutageLessThanNbUnitsStop(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);

            builder.build();
        }
    }
    else
    {
        *builder.data.NbTermesContraintesPourLesCoutsDeDemarrage += 2;
        builder.data.nombreDeContraintes++;
    }
}
