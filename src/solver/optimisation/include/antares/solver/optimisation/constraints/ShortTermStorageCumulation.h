// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

class ShortTermStorageCumulation final: ConstraintFactory
{
public:
    ShortTermStorageCumulation(ConstraintBuilder& builder,
                               ShortTermStorageCumulativeConstraintData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    void add(int pays);

private:
    ShortTermStorageCumulativeConstraintData& data;
};
