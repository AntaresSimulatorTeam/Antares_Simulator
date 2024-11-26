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

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationWithdrawal[h] + Withdrawal[h+1] - Withdrawal[h]  >= 0
void ShortTermStorageCostVariationWithdrawalBackward::buildConstraint(int index)
{
    builder.ShortTermCostVariationWithdrawal(index, 1.0)
      .ShortTermStorageWithdrawal(index, -1.0)
      .ShortTermStorageWithdrawal(index, 1.0, 1, builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationWithdrawalBackward::add(unsigned int pdt, unsigned int pays)
{
    addStorageConstraint("ShortTermStorageCostVariationWithdrawalBackward", pdt, pays);
}

int& ShortTermStorageCostVariationWithdrawalBackward::TargetConstraintIndex(int pdt, int index)
{
    return data.CorrespondanceCntNativesCntOptim[pdt]
      .ShortTermStorageCostVariationWithdrawalBackward[index];
}

bool ShortTermStorageCostVariationWithdrawalBackward::IsConstraintEnabled(
  const ShortTermStorage::PROPERTIES& properties)
{
    return properties.penalizeVariationWithdrawal;
}
