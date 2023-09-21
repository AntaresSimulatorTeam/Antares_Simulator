#pragma once
#include "ConstraintBuilder.h"

class BindingConstraintDay : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int cntCouplante);
};
