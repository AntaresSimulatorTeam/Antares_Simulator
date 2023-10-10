#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Max Pumping' constraint type
 */
class MaxPumping : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);
};