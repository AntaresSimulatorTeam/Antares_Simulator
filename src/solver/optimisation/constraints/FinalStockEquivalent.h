#pragma once
#include "ConstraintBuilder.h"

class FinalStockEquivalent : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pays);
};
