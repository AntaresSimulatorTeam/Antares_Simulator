#pragma once
#include "ConstraintGroup.h"
#include "HydroPower.h"

class HydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void buildConstraints() override;

private:
    HydroPowerData GetHydroPowerDataFromProblemHebdo();
};