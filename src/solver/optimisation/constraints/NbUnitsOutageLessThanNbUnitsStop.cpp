#include "NbUnitsOutageLessThanNbUnitsStop.h"

void NbUnitsOutageLessThanNbUnitsStop::add(
  int pays,
  std::shared_ptr<NbUnitsOutageLessThanNbUnitsStopData> data)
{
    if (!data->Simulation)
    {
        data->NumeroDeContrainteDesContraintesDeDureeMinDeMarche[data->cluster] = -1;

        builder->updateHourWithinWeek(data->pdt)
          .NumberBreakingDownDispatchableUnits(data->cluster, 1.0)
          .NumberStoppingDispatchableUnits(data->cluster, -1.0)
          .lessThan();

        if (builder->NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder->data->NomDesContraintes);
            namer.UpdateArea(builder->data->NomsDesPays[pays]);
            namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + data->pdt);
            namer.NbUnitsOutageLessThanNbUnitsStop(
              builder->data->nombreDeContraintes,
              data->PaliersThermiquesDuPays.NomsDesPaliersThermiques[data->clusterIndex]);

            builder->build();
        }
    }
    else
    {
        *builder->data->NbTermesContraintesPourLesCoutsDeDemarrage += 2;
        builder->data->nombreDeContraintes++;
    }
}
