#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"
/**
 * @brief Group of Pmin/Pmax constraints
 *
 */
class PMinMaxDispatchableGenerationGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

    /*TODO Rename this*/
    void Build() override;
};