/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationSum.h"

void HydroPowerSmoothingUsingVariationSum::add(int pays,
                                               const int nombreDePasDeTempsPourUneOptimisation)
{
    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int pdt1 = pdt + 1;
        if (pdt1 >= nombreDePasDeTempsPourUneOptimisation)
        {
            pdt1 = 0;
        }
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.HydroPowerSmoothingUsingVariationSum(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .HydProd(pays, 1.0)
          .updateHourWithinWeek(pdt1)
          .HydProd(pays, -1.0)
          .updateHourWithinWeek(pdt)
          .HydProdDown(pays, -1.0)
          .HydProdUp(pays, 1.0)
          .equalTo()
          .build();
    }
}
