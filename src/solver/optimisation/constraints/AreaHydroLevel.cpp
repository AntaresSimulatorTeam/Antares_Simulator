#include "AreaHydroLevel.h"

void AreaHydroLevel::add(int pays, int pdt, std::shared_ptr<AreaHydroLevelData> data)
{
    data->NumeroDeContrainteDesNiveauxPays[pays] = builder->data->nombreDeContraintes;
    if (data->SuiviNiveauHoraire)
    {
        builder->updateHourWithinWeek(pdt).HydroLevel(pays, 1.0);
        if (pdt > 0)
        {
            builder->updateHourWithinWeek(pdt - 1).HydroLevel(pays, -1.0);
        }
        ConstraintNamer namer(builder->data->NomDesContraintes);

        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
        namer.AreaHydroLevel(builder->data->nombreDeContraintes);
        data->NumeroDeContrainteDesNiveauxPays[pays] = builder->data->nombreDeContraintes;
        builder->updateHourWithinWeek(pdt)
          .HydProd(pays, 1.0)
          .Pumping(pays, -data->PumpingRatio)
          .Overflow(pays, 1.)
          .equalTo()
          .build();
    }
    else
        data->NumeroDeContrainteDesNiveauxPays[pays] = -1;
}