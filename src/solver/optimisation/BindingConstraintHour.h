#pragma once

#include "new_constraint_builder.h"
struct BindingConstraintHourData : public BindingConstraintData
{
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};
class BindingConstraintHour : private NewConstraint
{
    public:
        using NewConstraint::NewConstraint;
        void add(int pdt, int cntCouplante, std::shared_ptr<BindingConstraintHourData> data);
};
