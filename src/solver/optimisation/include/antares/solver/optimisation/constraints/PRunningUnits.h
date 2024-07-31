#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'PRunningUnits' Constraint type
 */
class PRunningUnits : private ConstraintFactory
{
public:
    PRunningUnits(ConstraintBuilder& builder, ReserveData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param reserve : capacity reservation
     * @param isUpReserve : true if ReserveUp, false if ReserveDown
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     */
    void add(int pays, int reserve, int cluster, int pdt, bool isUpReserve);

private:
    ReserveData& data;
};
