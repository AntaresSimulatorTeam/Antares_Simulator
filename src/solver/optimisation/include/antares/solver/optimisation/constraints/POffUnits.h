#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'POffUnits' Constraint type
 */
class POffUnits : private ConstraintFactory
{
public:
    POffUnits(ConstraintBuilder& builder, ReserveData& data) :
     ConstraintFactory(builder), data(data)
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
