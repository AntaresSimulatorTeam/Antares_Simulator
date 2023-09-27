#pragma once
#include "ConstraintGroup.h"
#include "BindingConstraintDay.h"

class BindingConstraintDayGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<BindingConstraintDayData> GetBindingConstraintDayDataFromProblemHebdo(
      int cntCouplante);
};