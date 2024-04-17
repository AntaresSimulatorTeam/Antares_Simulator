#pragma once
#include "ConstraintBuilder.h"

struct POutReserveData
{
    bool Simulation;
    ALL_AREA_RESERVES& areaReserves;
    std::vector<PALIERS_THERMIQUES> thermalClusters;
};

/*
 * represent 'POutCapacityThreasholds' Constraint type
 */
class POutCapacityThreasholds : private ConstraintFactory
{
public:
    POutCapacityThreasholds(ConstraintBuilder& builder, POutReserveData& data) :
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
    POutReserveData& data;
};
