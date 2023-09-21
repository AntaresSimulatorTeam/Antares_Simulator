#pragma once
#include "ConstraintBuilder.h"

class FlowDissociation : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pdt, int interco);
};
