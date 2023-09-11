#pragma once
#include "constraint_builder.h"

class AreaBalance : private Constraint
{
public:
    using Constraint::Constraint;

    void add(int pdt,
             int pays,
             std::vector<int>& NumeroDeContrainteDesBilansPays,
             std::vector<int>& InjectionVariable,
             std::vector<int>& WithdrawalVariable);
};