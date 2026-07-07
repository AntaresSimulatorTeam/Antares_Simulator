// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'ThermalReserveParticipation' Constraint type
 */
class ThermalReserveParticipation: private ConstraintFactory
{
public:
    ThermalReserveParticipation(ConstraintBuilder& builder, ReserveData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param reserve : capacity reservation
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     */
    void add(int pays, int reserve, int cluster, int pdt);

private:
    ReserveData& data;
};
