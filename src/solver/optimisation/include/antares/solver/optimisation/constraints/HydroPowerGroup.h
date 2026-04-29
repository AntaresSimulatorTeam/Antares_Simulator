// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintGroup.h"
#include "HydroPower.h"

class HydroPowerGroup final: public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void BuildConstraints() override;

private:
    HydroPowerData GetHydroPowerDataFromProblemHebdo();
};
