
#pragma once
#include "constraint_builder.h"

struct FictitiousLoad : public Constraint
{
    using Constraint::Constraint;
    void add(int pdt, int pays);
};
