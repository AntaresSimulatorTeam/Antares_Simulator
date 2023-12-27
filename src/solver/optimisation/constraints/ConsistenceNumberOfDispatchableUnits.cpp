#include "ConsistenceNumberOfDispatchableUnits.h"

void ConsistenceNumberOfDispatchableUnits::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder.data.NombreDePasDeTempsPourUneOptimisation;

        auto cluster = data.PaliersThermiquesDuPays[pays]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        int Pdtmoins1 = pdt - 1;
        if (Pdtmoins1 < 0)
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;

        builder.updateHourWithinWeek(pdt)
          .NumberOfDispatchableUnits(cluster, 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .NumberOfDispatchableUnits(cluster, -1)
          .updateHourWithinWeek(pdt)
          .NumberStartingDispatchableUnits(cluster, -1)
          .NumberStoppingDispatchableUnits(cluster, 1)
          .equalTo();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.ConsistenceNODU(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);

            builder.build();
        }
    }
    else
    {
        *builder.data.NbTermesContraintesPourLesCoutsDeDemarrage += 4;
        builder.data.nombreDeContraintes++;
    }
}
