#pragma once
#include "constraint_builder.h"

struct HydroPower : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};
