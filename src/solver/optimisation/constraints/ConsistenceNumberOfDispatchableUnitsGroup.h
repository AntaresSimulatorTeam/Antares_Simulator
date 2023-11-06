#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"

class ConsistenceNumberOfDispatchableUnitsGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    /*TODO Rename this*/
    void Build() override;
};