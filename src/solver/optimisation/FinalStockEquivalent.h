#pragma once
#include "constraint_builder.h"

struct FinalStockEquivalent : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
