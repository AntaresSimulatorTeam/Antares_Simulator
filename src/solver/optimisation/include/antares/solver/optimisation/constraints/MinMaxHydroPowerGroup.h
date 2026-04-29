// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintGroup.h"
#include "MaxHydroPower.h"
#include "MinHydroPower.h"

class MinMaxHydroPowerGroup final: public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void BuildConstraints() override;

private:
    MinHydroPowerData GetMinHydroPowerData();
    MaxHydroPowerData GetMaxHydroPowerData();
};
