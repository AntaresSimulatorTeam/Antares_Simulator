#pragma once
#include "ConstraintBuilder.h"

class MinHydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};
