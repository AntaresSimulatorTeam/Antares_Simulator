/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <vector>
#include <utility>
#include <cstdint>
#include <limits>

namespace Antares::Optimization
{
using VarIndex = uint32_t;

struct LinearExpression
{
    double offset{0.0};
    std::vector<std::pair<VarIndex, double>> coef_per_index; // (index variable, coefficient)

    // API de confort
    const auto& coefPerIndex() const { return coef_per_index; }
};

struct LinearConstraint
{
    double lb{-std::numeric_limits<double>::infinity()};
    double ub{ std::numeric_limits<double>::infinity()};
    int timeStep{0}; // si pertinent
    std::vector<std::pair<VarIndex, double>> coef_per_index;
};

} // namespace Antares::Optimization
