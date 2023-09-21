#pragma once
#include "ConstraintBuilder.h"

class AreaHydroLevel : private ConstraintFactory
{
    public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays, int pdt);
};