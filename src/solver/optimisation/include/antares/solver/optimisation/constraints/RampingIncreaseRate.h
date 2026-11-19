// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'RampingIncreaseRate' Constraint type
 */
class RampingIncreaseRate: private ConstraintFactory
{
public:
    RampingIncreaseRate(ConstraintBuilder& builder, StartUpCostsData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param index : local thermal-cluster index within the area
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, int index, int pdt);

private:
    StartUpCostsData& data;
};
