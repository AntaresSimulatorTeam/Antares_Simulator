// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct AreaHydroLevelData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
};

/*!
 * represent 'Area Hydraulic Level' constraint type
 */

class AreaHydroLevel final: private ConstraintFactory
{
public:
    AreaHydroLevel(ConstraintBuilder& builder, AreaHydroLevelData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pays, int pdt);

private:
    AreaHydroLevelData& data;
};
