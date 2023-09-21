#pragma once
#include "ConstraintBuilder.h"

class MaxHydroPower : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};