#pragma once
#include "new_constraint_builder.h"

/*!
 * represent 'Hydraulic Power Smoothing Using Variation Max Down' constraint type
 */

class HydroPowerSmoothingUsingVariationMaxDown : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */
    void add(int pays, int pdt);
};
