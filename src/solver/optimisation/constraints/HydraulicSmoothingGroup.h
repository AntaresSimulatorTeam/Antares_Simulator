#pragma once
#include "ConstraintGroup.h"

class HydraulicSmoothingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
};