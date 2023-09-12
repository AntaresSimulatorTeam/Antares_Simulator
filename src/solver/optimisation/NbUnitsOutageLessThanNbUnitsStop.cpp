#include "NbUnitsOutageLessThanNbUnitsStop.h"

void NbUnitsOutageLessThanNbUnitsStop::add(int pays,
                                           int cluster,
                                           int clusterIndex,
                                           int pdt,
                                           bool Simulation,
                                           NbUnitsOutageLessThanNbUnitsStopData& data)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        const int DureeMinimaleDArretDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];

        data.NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster] = -1;

        builder.updateHourWithinWeek(pdt)
          .include(Variable::NumberBreakingDownDispatchableUnits(cluster), 1.0)
          .include(Variable::NumberStoppingDispatchableUnits(cluster), -1.0)
          .lessThan();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.NbUnitsOutageLessThanNbUnitsStop(
              builder.data.nombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);

            builder.build();
        }
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage += 4;
        builder.data.nombreDeContraintes++;
    }
}
