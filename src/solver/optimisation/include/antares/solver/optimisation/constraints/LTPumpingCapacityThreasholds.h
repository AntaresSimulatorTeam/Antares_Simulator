#pragma once
#include "ConstraintBuilder.h"

/*
 * represent 'LTPumpingCapacityThreasholds' Constraint type
 */
class LTPumpingCapacityThreasholds : private ConstraintFactory
{
public:
    LTPumpingCapacityThreasholds(ConstraintBuilder& builder, ReserveData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     */
    void add(int pays, int cluster, int pdt);

private:
    ReserveData& data;
};
