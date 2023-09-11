#pragma once
#include "constraint_builder.h"

struct FinalStockEquivalentData
{
    const int pdt;
    const bool AccurateWaterValue;
    const bool DirectLevelAccess;
    std::vector<int>& NumeroDeContrainteEquivalenceStockFinal;
};

class FinalStockEquivalent : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pays, FinalStockEquivalentData& data);
};
