// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationInjection[h] + Injection[h+1] - Injection[h]  >= 0
void ShortTermStorageCostVariationInjectionBackward::buildConstraint(int index)
{
    builder.ShortTermCostVariationInjection(index, 1.0)
      .ShortTermStorageInjection(index, -1.0)
      .ShortTermStorageInjection(index, 1.0, 1, builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationInjectionBackward::add(unsigned int pdt, unsigned int pays)
{
    addStorageConstraint("ShortTermStorageCostVariationInjectionBackward", pdt, pays);
}

int& ShortTermStorageCostVariationInjectionBackward::
  ShortTermStorageCostVariationInjectionBackward::TargetConstraintIndex(int pdt, int index)
{
    return data.CorrespondanceCntNativesCntOptim[pdt]
      .ShortTermStorageCostVariationInjectionBackward[index];
}

bool ShortTermStorageCostVariationInjectionBackward::IsConstraintEnabled(
  const PROPERTIES& properties)
{
    return properties.penalizeVariationInjection;
}
