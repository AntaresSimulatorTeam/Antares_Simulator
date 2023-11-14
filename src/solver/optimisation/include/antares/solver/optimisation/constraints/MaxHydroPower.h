#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Max Hydraulic Power' constraint type
 */
class MaxHydroPower : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);
};