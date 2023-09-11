
#pragma once
#include "constraint_builder.h"

class FictitiousLoad : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int pays, std::vector<int>& NumeroDeContraintePourEviterLesChargesFictives);
};
