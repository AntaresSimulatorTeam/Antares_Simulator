#pragma once
#include "ConstraintBuilder.h"

class FlowDissociation : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int interco);
};
