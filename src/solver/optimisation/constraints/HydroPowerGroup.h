#pragma once
#include "ConstraintGroup.h"
#include "HydroPower.h"

class HydroPowerGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void BuildConstraints() override;

private:
    HydroPowerData GetHydroPowerDataFromProblemHebdo();
};