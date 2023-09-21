#pragma once
#include "ConstraintBuilder.h"

class AreaBalance : private Constraint
{
public:
    using Constraint::Constraint;

    void add(int pdt, int pays);
};