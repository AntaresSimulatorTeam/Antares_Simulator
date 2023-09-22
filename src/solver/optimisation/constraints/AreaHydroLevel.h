#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Area Hydraulic Level' constraint type
 */
class AreaHydroLevel : private ConstraintFactory
{
    public:
    using ConstraintFactory::ConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pays, int pdt);
};