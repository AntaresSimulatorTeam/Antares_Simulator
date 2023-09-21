#pragma once
#include "ConstraintBuilder.h"

class AreaBalance : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    void add(int pdt, int pays);
};