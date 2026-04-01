#pragma once
#include "ConstraintBuilder.h"

/*
 * represent 'HydroStoreCapacityThresholds' Constraint type
 */
class HydroStoreCapacityThresholds: private ConstraintFactory
{
public:
    HydroStoreCapacityThresholds(ConstraintBuilder& builder, ReserveData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param pdt : timestep
     */
    void add(int pays, int pdt);

private:
    ReserveData& data;
};
