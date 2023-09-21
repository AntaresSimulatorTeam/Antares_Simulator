#pragma once
#include "ConstraintBuilder.h"

class BindingConstraintHour : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pdt, int cntCouplante);
};
