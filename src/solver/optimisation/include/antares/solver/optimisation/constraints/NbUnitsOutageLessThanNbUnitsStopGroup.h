// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "NbUnitsOutageLessThanNbUnitsStop.h"

/**
 * @brief Group of NbUnitsOutageLessThanNbUnitsStop constraints
 *
 */

class NbUnitsOutageLessThanNbUnitsStopGroup final: public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    void BuildConstraints() override;

private:
    NbUnitsOutageLessThanNbUnitsStopData GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo();
};
