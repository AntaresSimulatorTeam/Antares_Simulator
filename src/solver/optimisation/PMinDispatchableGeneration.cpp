#include "PMinDispatchableGeneration.h"

void PMinDispatchableGeneration::add(int pays,
                                     int cluster,
                                     int clusterIndex,
                                     int pdt,
                                     bool Simulation,
                                     StartUpCostsData& data)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        double pminDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[clusterIndex];

        builder.updateHourWithinWeek(pdt)
          .include(Variable::DispatchableProduction(cluster), 1.0)
          .include(Variable::NODU(cluster), -pminDUnGroupeDuPalierThermique)
          .greaterThan();
        /*consider Adding naming constraint inside the builder*/
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.PMinDispatchableGeneration(
              builder.data.nombreDeContraintes,
              PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex]);
        }
          builder.build();
    }
    else
    {
        nbTermesContraintesPourLesCoutsDeDemarrage += 2;
        builder.data.nombreDeContraintes++;
    }
}
