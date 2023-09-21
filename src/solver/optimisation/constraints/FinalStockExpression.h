#pragma once

#include "ConstraintBuilder.h"

class FinalStockExpression : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};
