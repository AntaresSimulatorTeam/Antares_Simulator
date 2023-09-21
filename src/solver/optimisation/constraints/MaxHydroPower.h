#pragma once
#include "ConstraintBuilder.h"

class MaxHydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};