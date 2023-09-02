#pragma once
#include "constraint_builder.h"

struct BindingConstraintDay : public Constraint
{
    using Constraint::Constraint;
    void add(int cntCouplante);
};
