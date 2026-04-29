// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct MaxPumpingData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    std::vector<int>& NumeroDeContrainteMaxPompage;
};

/*!
 * represent 'Max Pumping' constraint type
 */

class MaxPumping final: private ConstraintFactory
{
public:
    MaxPumping(ConstraintBuilder& builder, MaxPumpingData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);

private:
    MaxPumpingData& data;
};
