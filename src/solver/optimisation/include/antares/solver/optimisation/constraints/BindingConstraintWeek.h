// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct BindingConstraintWeekData
{
    const std::vector<CONTRAINTES_COUPLANTES>& MatriceDesContraintesCouplantes;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};

/*!
 * represent 'Hourly Binding Constraint' type
 */

class BindingConstraintWeek final: private ConstraintFactory
{
public:
    BindingConstraintWeek(ConstraintBuilder& builder, BindingConstraintWeekData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param cntCouplante : the binding constraint number
     */
    void add(int cntCouplante);

private:
    BindingConstraintWeekData& data;
};
