#include "MinDownTime.h"

void MinDownTime::add(int pays, int index, int pdt)
{
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = data.PaliersThermiquesDuPays[pays].DureeMinimaleDArretDUnGroupeDuPalierThermique[index];
    auto cluster
      = data.PaliersThermiquesDuPays[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster]
      = -1;
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        builder.updateHourWithinWeek(pdt).NumberOfDispatchableUnits(cluster, 1.0);

        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder.updateHourWithinWeek(t1).NumberStoppingDispatchableUnits(cluster, 1.0);
        }
        builder.lessThan();
        if (builder.NumberOfVariables() > 1)
        {
            data.CorrespondanceCntNativesCntOptim[pdt]
              .NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster]
              = builder.data.nombreDeContraintes;
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.MinDownTime(builder.data.nombreDeContraintes,
                              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);

            builder.build();
        }
    }
    else
    {
        *builder.data.NbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + DureeMinimaleDArretDUnGroupeDuPalierThermique;
        builder.data.nombreDeContraintes++;
    }
}
