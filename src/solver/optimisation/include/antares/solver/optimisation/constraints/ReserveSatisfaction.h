// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

/*
 * represent 'ReserveSatisfaction' Constraint type
 */
class ReserveSatisfaction: private ConstraintFactory
{
public:
    ReserveSatisfaction(ConstraintBuilder& builder, ReserveData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param reserve : index of the reserve inside area
     * @param pdt : timestep
     */
    void add(int pays, int reserve, int pdt);

private:
    ReserveData& data;
};
