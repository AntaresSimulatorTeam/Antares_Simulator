// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxDown.h"

void HydroPowerSmoothingUsingVariationMaxDown::add(int pays, int pdt)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxDown(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .HydProd(pays, 1.0)
      .updateHourWithinWeek(0)
      .HydProdDown(pays, -1.0)
      .lessThan()
      .build();
}
