#pragma once

#include "ConstraintBuilder.h"

class FinalStockExpression : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays);
};
