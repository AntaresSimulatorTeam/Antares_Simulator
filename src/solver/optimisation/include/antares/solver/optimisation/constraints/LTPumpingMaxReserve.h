#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'LTPumpingMaxReserve' Constraint type
 */
class LTPumpingMaxReserve : private ConstraintFactory
{
public:
    LTPumpingMaxReserve(ConstraintBuilder& builder, ReserveData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param reserve : capacity reservation
     * @param cluster : local index of the cluster
     * @param pdt : timestep
     * @param isUpReserve : true if ReserveUp, false if ReserveDown
     */
    void add(int pays, int reserve, int cluster, int pdt, bool isUpReserve);

private:
    ReserveData& data;
};
