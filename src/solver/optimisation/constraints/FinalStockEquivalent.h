#pragma once
#include "ConstraintBuilder.h"

struct FinalStockEquivalentData
{
    const bool AccurateWaterValue;
    const bool DirectLevelAccess;
    std::vector<int>& NumeroDeContrainteEquivalenceStockFinal;
};

/*!
 * represent 'Final Stock Equivalent' constraint type
 */
class FinalStockEquivalent : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<FinalStockEquivalentData> data);
};
