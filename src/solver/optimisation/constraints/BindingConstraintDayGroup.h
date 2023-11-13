#pragma once
#include "ConstraintGroup.h"
#include "BindingConstraintDay.h"

class BindingConstraintDayGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
    BindingConstraintDayData GetBindingConstraintDayDataFromProblemHebdo(int cntCouplante);
};