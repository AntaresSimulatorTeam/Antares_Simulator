#pragma once
#include "ConstraintGroup.h"
#include "MaxPumping.h"

class MaxPumpingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void BuildConstraints() override;

private:
    MaxPumpingData GetMaxPumpingData();
};