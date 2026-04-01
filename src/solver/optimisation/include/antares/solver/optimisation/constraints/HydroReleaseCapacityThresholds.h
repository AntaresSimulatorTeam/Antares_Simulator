#pragma once
#include "ConstraintBuilder.h"

/*
 * represent 'HydroReleaseCapacityThresholds' Constraint type
 */
class HydroReleaseCapacityThresholds: private ConstraintFactory
{
public:
    HydroReleaseCapacityThresholds(ConstraintBuilder& builder, ReserveData& data):
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
