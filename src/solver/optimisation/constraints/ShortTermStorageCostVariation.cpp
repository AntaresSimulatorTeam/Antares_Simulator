// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

void ShortTermStorageCostVariation::addStorageConstraint(const std::string& constraintName,
                                                         int pdt,
                                                         int pays)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);

    builder.updateHourWithinWeek(pdt);
    for (const auto& storage: data.ShortTermStorage[pays])
    {
        if (IsConstraintEnabled(storage))
        {
            namer.ShortTermStorageCostVariation(constraintName,
                                                builder.data.nombreDeContraintes,
                                                storage.name);
            const auto index = storage.clusterGlobalIndex;

            TargetConstraintIndex(pdt, index) = builder.data.nombreDeContraintes;

            buildConstraint(index);
        }
    }
}
