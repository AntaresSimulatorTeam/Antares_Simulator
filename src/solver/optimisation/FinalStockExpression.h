#pragma once

#include "constraint_builder.h"

struct FinalStockExpressionData
{
    const int& pdt;
    const bool& AccurateWaterValue;
    std::vector<int>& NumeroDeContrainteExpressionStockFinal;
};
class FinalStockExpression : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, FinalStockExpressionData& data);
};
