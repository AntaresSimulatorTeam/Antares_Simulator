#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'STReserveDownParticipation' Constraint type
 */
class STReserveDownParticipation : private ConstraintFactory
{
public:
    STReserveDownParticipation(ConstraintBuilder& builder, ReserveData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param reserve : capacity reservation
     * @param cluster : local index of the cluster
     * @param pdt : timestep
     */
    void add(int pays, int reserve, int cluster, int pdt);

private:
    ReserveData& data;
};
