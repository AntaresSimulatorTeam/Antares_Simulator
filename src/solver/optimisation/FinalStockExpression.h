#pragma once

#include "constraint_builder.h"

struct FinalStockExpression : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};
