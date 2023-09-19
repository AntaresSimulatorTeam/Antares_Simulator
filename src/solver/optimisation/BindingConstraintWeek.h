#pragma once
#include "new_constraint_builder.h"

struct BindingConstraintWeekData : public BindingConstraintData
{
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};

class BindingConstraintWeek : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int cntCouplante, std::shared_ptr<BindingConstraintWeekData> data);
};