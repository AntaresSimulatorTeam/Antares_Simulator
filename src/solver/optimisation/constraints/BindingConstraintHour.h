#pragma once
#include "ConstraintBuilder.h"

class BindingConstraintHour : private ConstraintFactory
{
    public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pdt, int cntCouplante);
};
