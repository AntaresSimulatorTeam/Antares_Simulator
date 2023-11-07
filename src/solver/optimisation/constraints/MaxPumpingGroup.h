#pragma once
#include "ConstraintGroup.h"
#include "MaxPumping.h"

class MaxPumpingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
    std::shared_ptr<MaxPumpingData> GetMaxPumpingData(uint32_t pays);
};