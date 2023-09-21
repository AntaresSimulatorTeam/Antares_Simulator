#pragma once
#include "constraint_builder.h"

class HydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};
