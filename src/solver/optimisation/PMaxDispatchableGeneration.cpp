#include "PMaxDispatchableGeneration.h"

void PMaxDispatchableGeneration::add(int pays,
                                     int cluster,
                                     int clusterIndex,
                                     int pdt,
                                     bool Simulation,
                                     StartUpCostsData& data)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        double pmaxDUnGroupeDuPalierThermique
          = PaliersThermiquesDuPays.PmaxDUnGroupeDuPalierThermique[clusterIndex];

        builder.updateHourWithinWeek(pdt)
          .include(Variable::DispatchableProduction(cluster), 1.0)
          .include(Variable::NODU(cluster), -pmaxDUnGroupeDuPalierThermique)
          .lessThan();
        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.PMaxDispatchableGeneration(
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