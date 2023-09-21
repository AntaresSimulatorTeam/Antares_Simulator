#pragma once
#include "ConstraintBuilder.h"

class BindingConstraintDay : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante);
};
