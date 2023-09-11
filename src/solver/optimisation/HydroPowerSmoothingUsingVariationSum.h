
#pragma once
#include "constraint_builder.h"
class HydroPowerSmoothingUsingVariationSum : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays , const int nombreDePasDeTempsPourUneOptimisation);
};