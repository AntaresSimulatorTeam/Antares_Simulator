#pragma once
#include "ConstraintBuilder.h"

class ShortTermStorageLevel : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pdt, int pays);
};
