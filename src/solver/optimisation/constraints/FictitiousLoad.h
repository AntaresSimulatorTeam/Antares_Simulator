
#pragma once
#include "ConstraintBuilder.h"

class FictitiousLoad : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pdt, int pays);
};
