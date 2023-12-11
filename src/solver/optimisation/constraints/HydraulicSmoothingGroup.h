#pragma once
#include "ConstraintGroup.h"

class HydraulicSmoothingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void buildConstraints() override;

private:
};