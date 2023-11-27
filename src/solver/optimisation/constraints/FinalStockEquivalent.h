#pragma once
#include "ConstraintBuilder.h"

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
    void add(int pays);
};
