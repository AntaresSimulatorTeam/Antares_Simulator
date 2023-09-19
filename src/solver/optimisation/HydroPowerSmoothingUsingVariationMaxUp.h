#pragma once
#include "new_constraint_builder.h"

class HydroPowerSmoothingUsingVariationMaxUp : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pays, int pdt);
};