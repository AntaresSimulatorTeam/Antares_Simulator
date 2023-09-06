#pragma once
#include "constraint_builder.h"

struct MinHydroPower : private Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
