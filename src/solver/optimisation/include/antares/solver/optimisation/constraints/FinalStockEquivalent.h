// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct FinalStockEquivalentData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    std::vector<int>& NumeroDeContrainteEquivalenceStockFinal;
};

/*!
 * represent 'Final Stock Equivalent' constraint type
 */
class FinalStockEquivalent final: private ConstraintFactory
{
public:
    FinalStockEquivalent(ConstraintBuilder& builder, FinalStockEquivalentData& data):
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
    FinalStockEquivalentData& data;
};
