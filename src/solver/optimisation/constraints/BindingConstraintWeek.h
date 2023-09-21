#pragma once
#include "ConstraintBuilder.h"

class BindingConstraintWeek : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante);
};