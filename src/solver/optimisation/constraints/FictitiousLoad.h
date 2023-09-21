
#pragma once
#include "ConstraintBuilder.h"

class FictitiousLoad : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int pays);
};
