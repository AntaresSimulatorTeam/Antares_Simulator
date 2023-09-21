#pragma once
#include "ConstraintBuilder.h"

class HydroPowerSmoothingUsingVariationMaxDown : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int pdt);
};