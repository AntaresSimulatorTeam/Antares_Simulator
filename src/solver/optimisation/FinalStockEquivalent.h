#pragma once
#include "constraint_builder.h"

class FinalStockEquivalent : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pays);
};
