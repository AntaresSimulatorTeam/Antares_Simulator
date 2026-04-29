// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

class ShortTermStorageLevel final: private ConstraintFactory
{
public:
    ShortTermStorageLevel(ConstraintBuilder& builder, ShortTermStorageData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    void add(int pdt, int pays);

private:
    ShortTermStorageData& data;
};
