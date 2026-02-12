// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "ConstraintBuilder.h"

struct MaxUnsupEnergyData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
};

/*!
 * Purpose of this constraint : be able to maximize the unsupplied energy variable with
 * other variables of optimization.
 */
class MaxUnsuppliedEnergy final: private ConstraintFactory
{
public:
    MaxUnsuppliedEnergy(ConstraintBuilder& builder, MaxUnsupEnergyData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(unsigned pdt, int pays);

private:
    MaxUnsupEnergyData& data;
};
