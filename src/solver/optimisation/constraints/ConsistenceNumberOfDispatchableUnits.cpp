#include "ConsistenceNumberOfDispatchableUnits.h"

void ConsistenceNumberOfDispatchableUnits::add(int pays, std::shared_ptr<StartUpCostsData> data)
{
    if (!data->Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation
          = builder->data->NombreDePasDeTempsPourUneOptimisation;

        int Pdtmoins1 = data->pdt - 1;
        if (Pdtmoins1 < 0)
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;

        builder->updateHourWithinWeek(data->pdt)
          .NumberOfDispatchableUnits(data->cluster, 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .NumberOfDispatchableUnits(data->cluster, -1)
          .updateHourWithinWeek(data->pdt)
          .NumberStartingDispatchableUnits(data->cluster, -1)
          .NumberStoppingDispatchableUnits(data->cluster, 1)
          .equalTo();

        if (builder->NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder->data->NomDesContraintes);
            namer.UpdateArea(builder->data->NomsDesPays[pays]);

            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + data->pdt);
            namer.ConsistenceNODU(
              builder->data->nombreDeContraintes,
              data->PaliersThermiquesDuPays.NomsDesPaliersThermiques[data->clusterIndex]);

            builder->build();
        }
    }
    else
    {
        *builder->data->NbTermesContraintesPourLesCoutsDeDemarrage += 4;
        builder->data->nombreDeContraintes++;
    }
}
