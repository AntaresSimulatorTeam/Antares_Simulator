#pragma once
#include "ConstraintBuilder.h"

class HydroPowerSmoothingUsingVariationMaxDown : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays, int pdt);
};