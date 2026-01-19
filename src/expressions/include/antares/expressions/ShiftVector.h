// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
