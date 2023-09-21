#pragma once
#include "constraint_builder.h"

class MaxHydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};