// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "MinDownTime.h"

/**
 * @brief Group of MinDownTime constraints
 *
 */

class MinDownTimeGroup final: public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    void BuildConstraints() override;

private:
    MinDownTimeData GetMinDownTimeDataFromProblemHebdo();
};
