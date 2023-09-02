#pragma once
#include "constraint_builder.h"

struct MinHydroPower : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
