
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct FictitiousLoadData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    const std::vector<bool>& DefaillanceNegativeUtiliserHydro;
};

/*!
 * represent 'Fictitious Load' constraint type
 */
class FictitiousLoad final: private ConstraintFactory
{
public:
    FictitiousLoad(ConstraintBuilder& builder, FictitiousLoadData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pdt, int pays);

private:
    FictitiousLoadData& data;
};
