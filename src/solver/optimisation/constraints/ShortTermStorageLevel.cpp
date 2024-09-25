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

#include "antares/solver/optimisation/constraints/ShortTermStorageLevel.h"

void ShortTermStorageLevel::add(int pdt, int pays)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    builder.updateHourWithinWeek(pdt);
    for (const auto& storage: data.ShortTermStorage[pays])
    {
        // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        namer.ShortTermStorageLevel(builder.data.nombreDeContraintes, storage.name);
        const auto index = storage.clusterGlobalIndex;
        data.CorrespondanceCntNativesCntOptim[pdt].ShortTermStorageLevelConstraint[index]
          = builder.data.nombreDeContraintes;

        builder.ShortTermStorageLevel(index, 1.0)
          .ShortTermStorageLevel(index,
                                 -1.0,
                                 -1,
                                 builder.data.NombreDePasDeTempsPourUneOptimisation)
          .ShortTermStorageInjection(index, -storage.injectionEfficiency)
          .ShortTermStorageWithdrawal(index, storage.withdrawalEfficiency)
          .equalTo()
          .build();
    }
}
