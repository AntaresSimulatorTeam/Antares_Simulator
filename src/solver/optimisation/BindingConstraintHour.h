#pragma once
#include "constraint_builder.h"

struct BindingConstraintHourData : public BindingConstraintData
{
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
};

class BindingConstraintHour : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pdt, int cntCouplante, BindingConstraintHourData& data);
};
