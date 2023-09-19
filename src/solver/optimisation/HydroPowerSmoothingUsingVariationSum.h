
#pragma once
#include "new_constraint_builder.h"

class HydroPowerSmoothingUsingVariationSum : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pays, const int nombreDePasDeTempsPourUneOptimisation);
};