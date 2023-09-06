#pragma once
#include "constraint_builder.h"

struct HydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays);
};
