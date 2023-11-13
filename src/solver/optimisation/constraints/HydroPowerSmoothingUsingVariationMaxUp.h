#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Hydraulic Power Smoothing Using Variation Max Up' constraint type
 */
class HydroPowerSmoothingUsingVariationMaxUp : private NewConstraintFactory
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
