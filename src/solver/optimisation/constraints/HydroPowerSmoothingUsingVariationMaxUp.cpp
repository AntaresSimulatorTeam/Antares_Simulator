#include "HydroPowerSmoothingUsingVariationMaxUp.h"

void HydroPowerSmoothingUsingVariationMaxUp::add(int pays, int pdt)
{
    ConstraintNamer namer(builder->data->NomDesContraintes);
    namer.UpdateArea(builder->data->NomsDesPays[pays]);
    namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxUp(builder->data->nombreDeContraintes);

    builder->updateHourWithinWeek(pdt)
      .HydProd(pays, 1.0)
      .updateHourWithinWeek(0)
      .HydProdUp(pays, -1.0)
      .greaterThan()
      .build();
}