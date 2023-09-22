#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Number of Dispatchable Units Min Bound Since Min Up Time' type
 */
class NbDispUnitsMinBoundSinceMinUpTime : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};