#pragma once
#include "ConstraintGroup.h"
#include "PMinDispatchableGeneration.h"
#include "PMaxDispatchableGeneration.h"

class AbstractStartUpCostsGroup : public ConstraintGroup
{
public:
    AbstractStartUpCostsGroup(ConstraintBuilder& builder, bool simulation) :
     simulation_(simulation), ConstraintGroup(builder)
    {
    }

        void Build() = 0;

    bool simulation_ = false;
    StartUpCostsData GetStartUpCostsDataFromProblemHebdo();
};