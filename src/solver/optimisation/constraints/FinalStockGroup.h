#pragma once
#include "ConstraintGroup.h"
#include "FinalStockEquivalent.h"
#include "FinalStockExpression.h"

class FinalStockGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    
    void BuildConstraints() override;

private:
    FinalStockEquivalentData GetFinalStockEquivalentData();
    FinalStockExpressionData GetFinalStockExpressionData();
};