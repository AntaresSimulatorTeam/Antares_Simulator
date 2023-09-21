#pragma once
#include "ConstraintBuilder.h"

class FinalStockEquivalent : private ConstraintFactory
{
    public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};
