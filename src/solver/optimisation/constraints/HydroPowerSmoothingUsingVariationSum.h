
#pragma once
#include "ConstraintBuilder.h"
class HydroPowerSmoothingUsingVariationSum : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};