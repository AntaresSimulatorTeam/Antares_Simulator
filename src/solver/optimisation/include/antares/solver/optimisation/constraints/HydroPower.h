// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct HydroPowerData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES>& CaracteristiquesHydrauliques;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteEnergieHydraulique;
};

/*!
 * represent 'Hydraulic Power' constraint type
 */

class HydroPower final: private ConstraintFactory
{
public:
    HydroPower(ConstraintBuilder& builder, HydroPowerData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);

public:
    HydroPowerData& data;
};
