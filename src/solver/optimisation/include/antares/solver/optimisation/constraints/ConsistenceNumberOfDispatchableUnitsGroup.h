// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"

/**
 * @brief Group of ConsistenceNumberOfDispatchableUnits constraints
 *
 */

class ConsistenceNumberOfDispatchableUnitsGroup final: public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    void BuildConstraints() override;
};
