#pragma once
#include "ConstraintBuilder.h"

class BindingConstraintWeek : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int cntCouplante);
};