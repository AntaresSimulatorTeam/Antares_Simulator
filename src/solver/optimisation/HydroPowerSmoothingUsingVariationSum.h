
#pragma once
#include "constraint_builder.h"
struct HydroPowerSmoothingUsingVariationSum : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};