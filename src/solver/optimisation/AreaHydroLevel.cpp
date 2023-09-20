#include "AreaHydroLevel.h"

void AreaHydroLevel::add(int pays, int pdt, std::shared_ptr<AreaHydroLevelData> data)
{
    data->NumeroDeContrainteDesNiveauxPays[pays] = builder->data->nombreDeContraintes;
    if (data->SuiviNiveauHoraire)
    {
        builder->updateHourWithinWeek(pdt).include(NewVariable::HydroLevel(pays), 1.0);
        if (pdt > 0)
        {
            builder->updateHourWithinWeek(pdt - 1).include(NewVariable::HydroLevel(pays), -1.0);
        }
        ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);

        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
        namer.AreaHydroLevel(builder->data->nombreDeContraintes);
        data->NumeroDeContrainteDesNiveauxPays[pays] = builder->data->nombreDeContraintes;
        builder->updateHourWithinWeek(pdt)
          .include(NewVariable::HydProd(pays), 1.0)
          .include(NewVariable::Pumping(pays), -data->PumpingRatio)
          .include(NewVariable::Overflow(pays), 1.)
          .equalTo()
          .build();
    }
    else
        data->NumeroDeContrainteDesNiveauxPays[pays] = -1;
}