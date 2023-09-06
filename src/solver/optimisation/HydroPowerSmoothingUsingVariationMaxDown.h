#pragma once
#include "constraint_builder.h"

class HydroPowerSmoothingUsingVariationMaxDown : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int pdt);
};