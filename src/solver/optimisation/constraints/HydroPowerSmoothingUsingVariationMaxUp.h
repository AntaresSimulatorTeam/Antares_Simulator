#pragma once
#include "ConstraintBuilder.h"

class HydroPowerSmoothingUsingVariationMaxUp : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int pdt);
};