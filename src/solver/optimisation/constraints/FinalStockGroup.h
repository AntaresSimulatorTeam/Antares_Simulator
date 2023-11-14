#pragma once
#include "ConstraintGroup.h"
#include "FinalStockEquivalent.h"
#include "FinalStockExpression.h"

class FinalStockGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void Build() override;

private:
    FinalStockEquivalentData GetFinalStockEquivalentData();
    FinalStockExpressionData GetFinalStockExpressionData();
};