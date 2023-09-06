#pragma once
#include "constraint_builder.h"

class BindingConstraintDay : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante);
};
