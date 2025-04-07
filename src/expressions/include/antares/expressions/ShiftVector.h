/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <algorithm>
#include <vector>

template<class T>
std::vector<T> shiftVector(const std::vector<T>& values, int shiftValue)
{
    const auto n = static_cast<int>(values.size());
    if (n == 0)
    {
        return {};
    }

    // Normalize shiftValue within bounds
    shiftValue = (shiftValue % n + n) % n;

    // Create a copy of the original vector
    auto shiftedValues = values;

    // Use std::rotate to perform the shift (left shift for positive values)
    std::rotate(shiftedValues.begin(), shiftedValues.begin() + shiftValue, shiftedValues.end());

    return shiftedValues;
}
