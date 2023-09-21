#pragma once
#include "ConstraintBuilder.h"

class AreaHydroLevel : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pays, int pdt);
};