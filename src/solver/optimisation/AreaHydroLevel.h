#pragma once
#include "constraint_builder.h"

class AreaHydroLevel : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pays, int pdt, std::vector<int>& NumeroDeContrainteDesNiveauxPays);
};