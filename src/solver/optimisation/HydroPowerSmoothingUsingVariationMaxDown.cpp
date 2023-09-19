#include "HydroPowerSmoothingUsingVariationMaxDown.h"

void HydroPowerSmoothingUsingVariationMaxDown::add(int pays, int pdt)
{
    ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);
    namer.UpdateArea(builder->data->NomsDesPays[pays]);
    namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxDown(builder->data->nombreDeContraintes);

    builder->updateHourWithinWeek(pdt)
      .include(NewVariable::HydProd(pays), 1.0)
      .updateHourWithinWeek(0)
      .include(NewVariable::HydProdDown(pays), -1.0)
      .lessThan()
      .build();
}