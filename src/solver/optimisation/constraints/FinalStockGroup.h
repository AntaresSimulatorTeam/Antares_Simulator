#pragma once
#include "ConstraintGroup.h"
#include "FinalStockEquivalent.h"
#include "FinalStockExpression.h"

class FinalStockGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    /*TODO Rename this*/
    void Build() override;

private:
    std::shared_ptr<FinalStockEquivalentData> GetFinalStockEquivalentData(uint32_t pays);
    std::shared_ptr<FinalStockExpressionData> GetFinalStockExpressionData(uint32_t pays);
};