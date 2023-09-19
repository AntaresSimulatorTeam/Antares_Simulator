#include "HydroPowerSmoothingUsingVariationMaxUp.h"

void HydroPowerSmoothingUsingVariationMaxUp::add(int pays, int pdt)
{
    ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);
    namer.UpdateArea(builder->data->NomsDesPays[pays]);
    namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxUp(builder->data->nombreDeContraintes);

    builder->updateHourWithinWeek(pdt)
      .include(NewVariable::HydProd(pays), 1.0)
      .updateHourWithinWeek(0)
      .include(NewVariable::HydProdUp(pays), -1.0)
      .greaterThan()
      .build();
}