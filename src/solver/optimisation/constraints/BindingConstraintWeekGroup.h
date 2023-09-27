#pragma once
#include "ConstraintGroup.h"
#include "BindingConstraintWeek.h"

class BindingConstraintWeekGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<BindingConstraintWeekData> GetBindingConstraintWeekDataFromProblemHebdo(
      int cntCouplante);
};