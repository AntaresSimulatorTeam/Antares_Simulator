#pragma once
#include "ConstraintBuilder.h"

class MaxPumping : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};