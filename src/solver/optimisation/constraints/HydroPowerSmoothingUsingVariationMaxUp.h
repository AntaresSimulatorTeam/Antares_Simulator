#pragma once
#include "ConstraintBuilder.h"

class HydroPowerSmoothingUsingVariationMaxUp : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays, int pdt);
};