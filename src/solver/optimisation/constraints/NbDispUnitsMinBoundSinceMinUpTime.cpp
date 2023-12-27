#include "NbDispUnitsMinBoundSinceMinUpTime.h"

void NbDispUnitsMinBoundSinceMinUpTime::add(int pays, int index, int pdt)
{
    auto cluster
      = data.PaliersThermiquesDuPays[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = data.PaliersThermiquesDuPays[pays].DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index];

    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
      = -1;
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        builder.updateHourWithinWeek(pdt).NumberOfDispatchableUnits(cluster, 1.0);

        for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder.updateHourWithinWeek(t1)
              .NumberStartingDispatchableUnits(cluster, -1.0)
              .NumberBreakingDownDispatchableUnits(cluster, 1.0);
        }

        builder.greaterThan();
        if (builder.NumberOfVariables() > 1)
        {
            data.CorrespondanceCntNativesCntOptim[pays]
              .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
              = builder.data.nombreDeContraintes;

            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.NbDispUnitsMinBoundSinceMinUpTime(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);
            builder.build();
        }
    }
    else
    {
        *builder.data.NbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + 2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
        builder.data.nombreDeContraintes++;
    }
}
