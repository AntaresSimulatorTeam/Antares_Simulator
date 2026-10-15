// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "PowerOutputVariationDecrease.h"
#include "PowerOutputVariationIncrease.h"

/**
 * @brief Group of PowerOutputVariationGroup constraints
 *
 */
class PowerOutputVariationGroup: public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    void BuildConstraints() override;
};
