// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <algorithm>
#include <vector>

template<class T>
std::vector<T> shiftVector(std::vector<T> values, int shift)
{
    const auto n = static_cast<int>(values.size());
    if (n == 0)
    {
        return {};
    }

    shift = (shift % n + n) % n; // Handle negative shifts and shifts larger than the vector size

    std::rotate(values.begin(), values.begin() + shift, values.end());
    return values;
}
