
#pragma once
#include "ConstraintBuilder.h"
class HydroPowerSmoothingUsingVariationSum : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};