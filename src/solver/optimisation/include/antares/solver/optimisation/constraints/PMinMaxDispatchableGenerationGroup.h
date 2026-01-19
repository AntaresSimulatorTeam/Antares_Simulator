// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "PMaxDispatchableGeneration.h"
#include "PMinDispatchableGeneration.h"

/**
 * @brief Group of Pmin/PmaxDispatchableGenerationGroup constraints
 *
 */
class PMinMaxDispatchableGenerationGroup final: public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;
    ~PMinMaxDispatchableGenerationGroup() = default;
    void BuildConstraints() override;
};
