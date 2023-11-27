#pragma once

#include "ConstraintBuilder.h"

/*!
 * represent 'Final Stock Expression' constraint type
 */
class FinalStockExpression : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);
};
