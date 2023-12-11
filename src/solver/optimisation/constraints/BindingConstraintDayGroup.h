#pragma once
#include "ConstraintGroup.h"
#include "BindingConstraintDay.h"

class BindingConstraintDayGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void buildConstraints() override;

private:
    BindingConstraintDayData GetBindingConstraintDayDataFromProblemHebdo();
};