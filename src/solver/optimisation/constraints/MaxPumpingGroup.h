#pragma once
#include "ConstraintGroup.h"
#include "MaxPumping.h"

class MaxPumpingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<MaxPumpingData> GetMaxPumpingData(uint32_t pays);
};