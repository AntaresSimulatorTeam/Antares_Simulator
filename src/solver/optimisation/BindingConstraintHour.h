#pragma once
#include "constraint_builder.h"

struct BindingConstraintHour : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int cntCouplante);
};
