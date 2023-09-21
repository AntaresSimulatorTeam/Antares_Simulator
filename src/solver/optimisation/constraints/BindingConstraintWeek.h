#pragma once
#include "constraint_builder.h"

class BindingConstraintWeek : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante);
};