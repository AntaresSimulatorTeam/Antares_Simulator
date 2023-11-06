#include "PMinDispatchableGeneration.h"

void PMinDispatchableGeneration::add(int pays, std::shared_ptr<StartUpCostsData> data)
{
    if (!data->Simulation)
    {
        double pminDUnGroupeDuPalierThermique
          = data->PaliersThermiquesDuPays.pminDUnGroupeDuPalierThermique[data->clusterIndex];

        builder->updateHourWithinWeek(data->pdt)
          .DispatchableProduction(data->cluster, 1.0)
          .NumberOfDispatchableUnits(data->cluster, -pminDUnGroupeDuPalierThermique)
          .greaterThan();
        /*consider Adding naming constraint inside the builder*/
        if (builder->NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder->data->NomDesContraintes);
            namer.UpdateArea(builder->data->NomsDesPays[pays]);

            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + data->pdt);
            namer.PMinDispatchableGeneration(
              builder->data->nombreDeContraintes,
              data->PaliersThermiquesDuPays.NomsDesPaliersThermiques[data->clusterIndex]);
        }
        builder->build();
    }
    else
    {
        *builder->data->NbTermesContraintesPourLesCoutsDeDemarrage += 2;
        builder->data->nombreDeContraintes++;
    }
}
