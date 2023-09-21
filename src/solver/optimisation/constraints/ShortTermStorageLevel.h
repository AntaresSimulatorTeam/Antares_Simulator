#pragma once
#include "ConstraintBuilder.h"

class ShortTermStorageLevel : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int pays);
};
