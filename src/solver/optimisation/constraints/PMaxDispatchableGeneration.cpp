#include "PMaxDispatchableGeneration.h"

void PMaxDispatchableGeneration::add(int pays, std::shared_ptr<StartUpCostsData> data)
{
    if (!data->Simulation)
    {
        double pmaxDUnGroupeDuPalierThermique
          = data->PaliersThermiquesDuPays.PmaxDUnGroupeDuPalierThermique[data->clusterIndex];

        builder->updateHourWithinWeek(data->pdt)
          .DispatchableProduction(data->cluster, 1.0)
          .NumberOfDispatchableUnits(data->cluster, -pmaxDUnGroupeDuPalierThermique)
          .lessThan();
        if (builder->NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder->data->NomDesContraintes);

            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + data->pdt);
            namer.UpdateArea(builder->data->NomsDesPays[pays]);

            namer.PMaxDispatchableGeneration(
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
