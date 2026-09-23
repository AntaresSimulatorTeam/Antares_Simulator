// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "RampingDecreaseRate.h"
#include "RampingIncreaseRate.h"

/**
 * @brief Group of RampingIncrease/DecreaseRateGroup constraints
 *
 */
class RampingIncreaseDecreaseRateGroup: public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    void BuildConstraints() override;
};
