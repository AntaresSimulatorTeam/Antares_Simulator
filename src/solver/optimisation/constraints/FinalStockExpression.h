#pragma once

#include "new_constraint_builder.h"

struct FinalStockExpressionData
{
    const bool AccurateWaterValue;
    std::vector<int>& NumeroDeContrainteExpressionStockFinal;
};

/*!
 * represent 'Final Stock Expression' constraint type
 */
class FinalStockExpression : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<FinalStockExpressionData> data);
};
