#include "NbDispUnitsMinBoundSinceMinUpTime.h"

void NbDispUnitsMinBoundSinceMinUpTime::add(int pays,
                                            int cluster,
                                            int clusterIndex,
                                            int pdt,
                                            bool Simulation,
                                            NbDispUnitsMinBoundSinceMinUpTimeData& data)
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = PaliersThermiquesDuPays.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[clusterIndex];

    data.NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster] = -1;
    if (!Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
          = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[clusterIndex]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique;

        builder.updateHourWithinWeek(pdt).include(Variable::NODU(cluster), 1.0);

        for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;

            builder.updateHourWithinWeek(t1)
              .include(Variable::NumberStartingDispatchableUnits(cluster), -1.0)
              .include(Variable::NumberBreakingDownDispatchableUnits(cluster), 1.0);
        }

        builder.greaterThan();
        if (builder.NumberOfVariables() > 1)
        {
            data.NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
              = builder.data.nombreDeContraintes;

            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.NbDispUnitsMinBoundSinceMinUpTime(
              builder.data.nombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
            builder.build();
        }
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage
          += 1 + 2 * DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
        builder.data.nombreDeContraintes++;
    }
}
