#include "ConsistenceNODU.h"

void ConsistenceNODU::add(int pays,
                          int cluster,
                          int clusterIndex,
                          int pdt,
                          bool Simulation,
                          StartUpCostsData& data)
{
    if (!Simulation)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];

        int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        int Pdtmoins1 = pdt - 1;
        if (Pdtmoins1 < 0)
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;

        builder.updateHourWithinWeek(pdt)
          .include(Variable::NODU(cluster), 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .include(Variable::NODU(cluster), -1)
          .updateHourWithinWeek(pdt)
          .include(Variable::NumberStartingDispatchableUnits(cluster), -1)
          .include(Variable::NumberStoppingDispatchableUnits(cluster), 1)
          .equalTo();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.ConsistenceNODU(builder.data.nombreDeContraintes,
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
