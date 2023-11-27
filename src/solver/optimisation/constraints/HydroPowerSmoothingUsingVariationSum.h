
#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Hydraulic Power Smoothing Using Variation Sum' constraint type
 */
class HydroPowerSmoothingUsingVariationSum : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays);
};