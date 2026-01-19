// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
  const PROPERTIES& properties)
{
    return properties.penalizeVariationWithdrawal;
}
