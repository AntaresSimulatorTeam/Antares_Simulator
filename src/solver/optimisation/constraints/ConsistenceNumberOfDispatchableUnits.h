#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Consistence Number Of Dispatchable Units Constraint' type
 */
class ConsistenceNumberOfDispatchableUnits : private ConstraintFactory
{
public:
    ConsistenceNumberOfDispatchableUnits(ConstraintBuilder& builder,
                                         StartUpCostsData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, int index, int pdt);

private:
    StartUpCostsData& data;
};