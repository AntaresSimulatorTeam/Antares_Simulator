#pragma once
#include "ConstraintBuilder.h"

class MinHydroPower : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};
