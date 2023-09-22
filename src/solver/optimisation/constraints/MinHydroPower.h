#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Min Hydraulic Power' constraint type
 */
class MinHydroPower : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);
};
