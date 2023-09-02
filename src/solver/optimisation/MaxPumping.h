#pragma once
#include "constraint_builder.h"

struct MaxPumping : public Constraint
{
    using Constraint::Constraint;
    void add(int pays);
};