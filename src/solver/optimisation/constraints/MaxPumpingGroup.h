#pragma once
#include "ConstraintGroup.h"
#include "MaxPumping.h"

class MaxPumpingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
    MaxPumpingData GetMaxPumpingData();
};