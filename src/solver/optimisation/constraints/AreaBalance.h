#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Area Balance' constraint type
 */
class AreaBalance : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pdt, int pays);
};