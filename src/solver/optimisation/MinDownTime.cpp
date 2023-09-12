#include "MinDownTime.h"

void MinDownTime::add(int pays,
                      int cluster,
                      int clusterIndex,
                      int pdt,
                      bool Simulation,
                      MinDownTimeData& data)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex];

    data.NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster] = -1;
    if (!Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique;

        builder.updateHourWithinWeek(pdt).include(Variable::NODU(cluster), 1.0);

        for (int k = pdt - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder.updateHourWithinWeek(t1).include(
              Variable::NumberStoppingDispatchableUnits(cluster), 1.0);
        }
        builder.lessThan();
        if (builder.NumberOfVariables() > 1)
        {
            data.NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster]
              = builder.data.nombreDeContraintes;
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.MinDownTime(builder.data.nombreDeContraintes,
                              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);

            builder.build();
        }
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + DureeMinimaleDArretDUnGroupeDuPalierThermique;
        builder.data.nombreDeContraintes++;
    }
}
