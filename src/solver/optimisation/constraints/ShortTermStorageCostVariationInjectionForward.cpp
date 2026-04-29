// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationInjection[h] - Injection[h+1] + Injection[h]  >= 0
void ShortTermStorageCostVariationInjectionForward::buildConstraint(int index)
{
    builder.ShortTermCostVariationInjection(index, 1.0)
      .ShortTermStorageInjection(index, 1.0)
      .ShortTermStorageInjection(index, -1.0, 1, builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationInjectionForward::add(unsigned int pdt, unsigned int pays)
{
    addStorageConstraint("ShortTermStorageCostVariationInjectionForward", pdt, pays);
}

int& ShortTermStorageCostVariationInjectionForward::ShortTermStorageCostVariationInjectionForward::
  TargetConstraintIndex(int pdt, int index)
{
    return data.CorrespondanceCntNativesCntOptim[pdt]
      .ShortTermStorageCostVariationInjectionForward[index];
}

bool ShortTermStorageCostVariationInjectionForward::IsConstraintEnabled(
  const PROPERTIES& properties)
{
    return properties.penalizeVariationInjection;
}
