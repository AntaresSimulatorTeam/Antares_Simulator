#pragma once
#include "ConstraintBuilder.h"

class MaxPumping : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};