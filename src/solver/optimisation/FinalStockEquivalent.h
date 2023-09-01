#pragma once
#include "constraintsbuilder.h"

struct FinalStockEquivalent : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
