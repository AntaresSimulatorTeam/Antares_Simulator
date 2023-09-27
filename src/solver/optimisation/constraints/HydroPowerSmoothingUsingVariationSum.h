
#pragma once
#include "new_constraint_builder.h"
/*!
 * represent 'Hydraulic Power Smoothing Using Variation Sum' constraint type
 */

class HydroPowerSmoothingUsingVariationSum : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, const int nombreDePasDeTempsPourUneOptimisation);
};
