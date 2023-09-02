#pragma once
#include "constraint_builder.h"

struct FlowDissociation : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int interco);
};
