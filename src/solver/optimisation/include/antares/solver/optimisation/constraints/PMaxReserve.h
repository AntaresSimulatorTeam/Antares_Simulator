#pragma once
#include "ConstraintBuilder.h"

struct PMaxReserveData
{
    bool Simulation;
    ALL_AREA_RESERVES& areaReserves;
    std::vector<PALIERS_THERMIQUES> thermalClusters;
};

/*!
 * represent 'ReserveParticipation' Constraint type
 */
class PMaxReserve : private ConstraintFactory
{
public:
    PMaxReserve(ConstraintBuilder& builder, PMaxReserveData& data) :
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
    PMaxReserveData& data;
};
