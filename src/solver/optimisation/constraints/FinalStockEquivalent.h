#pragma once
#include "new_constraint_builder.h"

struct FinalStockEquivalentData
{
    const bool AccurateWaterValue;
    const bool DirectLevelAccess;
    std::vector<int>& NumeroDeContrainteEquivalenceStockFinal;
};

/*!
 * represent 'Final Stock Equivalent' constraint type
 */
class FinalStockEquivalent : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, std::shared_ptr<FinalStockEquivalentData> data);
};
