#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"

/**
 * @brief Group of ConsistenceNumberOfDispatchableUnits constraints
 *
 */

class ConsistenceNumberOfDispatchableUnitsGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    /*TODO Rename this*/
    void Build() override;
};