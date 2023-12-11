#pragma once
#include "ConstraintGroup.h"
#include "BindingConstraintWeek.h"

class BindingConstraintWeekGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void buildConstraints() override;

private:
    BindingConstraintWeekData GetBindingConstraintWeekDataFromProblemHebdo();
};