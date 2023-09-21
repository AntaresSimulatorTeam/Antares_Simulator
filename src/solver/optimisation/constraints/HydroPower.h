#pragma once
#include "ConstraintBuilder.h"

class HydroPower : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};
