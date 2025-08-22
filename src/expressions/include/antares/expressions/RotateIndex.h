/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

/**
 * @brief Computes a rotated index within a bounded range of timesteps.
 *
 * This function calculates the new index for a given key after applying a
 * cyclic shift within a specified range of timesteps. The shift is normalized
 * to ensure it falls within the bounds of the range size, and the resulting
 * index is computed using modulo arithmetic to handle wrap-around cases.
 *
 * @param key The original index or key for which the rotated index is computed.
 * @param shift The amount by which the index should be rotated. This value
 *              can be positive or negative.
 * @param fillContext A context object providing information about the range
 *                    of timesteps, including the total number of timesteps
 *                    and the first timestep.
 *
 * @return The new index after applying the rotation.
 *
 * @example
 * // Example usage with visual representation:
 * Optimisation::LinearProblemApi::FillContext fillContext;
 * fillContext.NumberOfTimestep = 5; // Range size is 5
 * fillContext.firstTimeStep = 10;   // First timestep is 10
 * // Timesteps range: {10, 11, 12, 13, 14}
 *
 * unsigned key = 12;
 * int shift = 2;
 * int newIndex = rotatedIndex(key, shift, fillContext);
 *
 * // Step-by-step calculation:
 * // 1. Normalize shift: shift = (2 % 5 + 5) % 5 = 2
 * // 2. Compute relative position of key: key - firstTimeStep = 12 - 10 = 2
 * // 3. Apply shift: (2 + 2) % 5 = 4
 * // 4. Convert back to original range: 10 + 4 = 14
 * // newIndex will be 14
 *
 * @example
 * // Example with negative shift and visual representation:
 * Optimisation::LinearProblemApi::FillContext fillContext;
 * fillContext.NumberOfTimestep = 5; // Range size is 5
 * fillContext.firstTimeStep = 10;   // First timestep is 10
 * // Timesteps range: {10, 11, 12, 13, 14}
 *
 * unsigned key = 12;
 * int shift = -2;
 * int newIndex = rotatedIndex(key, shift, fillContext);
 *
 * // Step-by-step calculation:
 * // 1. Normalize shift: shift = (-2 % 5 + 5) % 5 = 3
 * // 2. Compute relative position of key: key - firstTimeStep = 12 - 10 = 2
 * // 3. Apply shift: (2 + 3) % 5 = 0
 * // 4. Convert back to original range: 10 + 0 = 10
 * // newIndex will be 10
 */
inline int rotatedIndex(unsigned key,
                        int shift,
                        const Antares::Optimisation::LinearProblemApi::FillContext& fillContext)
{
    unsigned rangeSize = fillContext.getLocalNumberOfTimeSteps();

    // Normalize shift within bounds (to prevent negative indexing)
    shift = (shift % static_cast<int>(rangeSize) + rangeSize) % static_cast<int>(rangeSize);

    // Compute which key's value should be assigned to `key`
    return fillContext.getLocalFirstTimeStep()
           + (key - fillContext.getLocalFirstTimeStep() + shift) % rangeSize;
}
