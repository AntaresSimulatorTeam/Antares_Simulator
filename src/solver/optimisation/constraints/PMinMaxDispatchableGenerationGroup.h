#pragma once
#include "AbstractStartUpCostsGroup.h"
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"

/**
 * @brief Group of Pmin/PmaxDispatchableGenerationGroup constraints
 *
 */
class PMinMaxDispatchableGenerationGroup : public AbstractStartUpCostsGroup
{
public:
    using AbstractStartUpCostsGroup::AbstractStartUpCostsGroup;

        void BuildConstraints() override;
};