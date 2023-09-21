#pragma once
#include "ConstraintBuilder.h"

class HydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};
