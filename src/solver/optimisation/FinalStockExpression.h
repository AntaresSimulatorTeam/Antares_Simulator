#pragma once

#include "constraint_builder.h"

struct FinalStockExpression : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
