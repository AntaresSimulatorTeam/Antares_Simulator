// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "BindingConstraintDay.h"
#include "ConstraintGroup.h"

class BindingConstraintDayGroup final: public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void BuildConstraints() override;

private:
    BindingConstraintDayData GetBindingConstraintDayDataFromProblemHebdo();
};
