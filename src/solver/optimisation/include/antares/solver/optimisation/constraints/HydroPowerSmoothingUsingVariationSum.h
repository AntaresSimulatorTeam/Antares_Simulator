
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Hydraulic Power Smoothing Using Variation Sum' constraint type
 */

class HydroPowerSmoothingUsingVariationSum final: private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     */
    void add(int pays, const int nombreDePasDeTempsPourUneOptimisation);
};
