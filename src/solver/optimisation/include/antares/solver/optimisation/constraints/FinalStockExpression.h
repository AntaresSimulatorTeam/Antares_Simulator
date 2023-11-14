#pragma once

#include "ConstraintBuilder.h"

struct FinalStockExpressionData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    std::vector<int>& NumeroDeContrainteExpressionStockFinal;
};

/*!
 * represent 'Final Stock Expression' constraint type
 */
class FinalStockExpression : private ConstraintFactory
{
public:
    FinalStockExpression(ConstraintBuilder& builder,
                         FinalStockExpressionData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);

private:
    FinalStockExpressionData& data;
};
