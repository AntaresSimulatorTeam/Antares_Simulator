#pragma once
#include "ConstraintGroup.h"

class HydraulicSmoothingGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
};