#pragma once
#include "new_constraint_builder.h"

/*!
 * represent 'Consistence Number Of Dispatchable Units Constraint' type
 */
class ConsistenceNumberOfDispatchableUnits : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, std::shared_ptr<StartUpCostsData> data);
};