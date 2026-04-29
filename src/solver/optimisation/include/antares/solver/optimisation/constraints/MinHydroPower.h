// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct MinHydroPowerData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMinEnergieHydraulique;
};

/*!
 * represent 'Min Hydraulic Power' constraint type
 */

class MinHydroPower final: private ConstraintFactory
{
public:
    MinHydroPower(ConstraintBuilder& builder, MinHydroPowerData& data):
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
    MinHydroPowerData& data;
};
