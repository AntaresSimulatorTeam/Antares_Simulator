/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

namespace Antares::Optimisation
{
/**
 * @brief Represents the time and scenario variation of a value.
 */
enum class VariabilityType : unsigned int
{
    CONSTANT_IN_TIME_AND_SCENARIO = 0,
    VARYING_IN_TIME_ONLY = 1,
    VARYING_IN_SCENARIO_ONLY = 2,
    VARYING_IN_TIME_AND_SCENARIO = 3
};

/**
 * @brief Combines two TimeIndex values.
 *
 * @param left The left operand.
 * @param right The right operand.
 *
 * @return The combined TimeIndex value.
 */
constexpr VariabilityType operator|(const VariabilityType& left, const VariabilityType& right)
{
    /*
     0 | x = x
     3 | x = 3
     1 | 1 = 1
     1 | 2 = 3
     2 | 2 = 2
     */
    return static_cast<VariabilityType>(static_cast<unsigned int>(left)
                                        | static_cast<unsigned int>(right));
}

constexpr VariabilityType variability(bool timedependent, bool scenariodependent)
{
    return static_cast<VariabilityType>((unsigned)scenariodependent << 1 | (unsigned)timedependent);
}

constexpr bool isTimeDependent(VariabilityType variability)
{
    return variability == VariabilityType::VARYING_IN_TIME_ONLY
           || variability == VariabilityType::VARYING_IN_TIME_AND_SCENARIO;
}

constexpr bool isScenarioDependent(VariabilityType variability)
{
    return variability == VariabilityType::VARYING_IN_SCENARIO_ONLY
           || variability == VariabilityType::VARYING_IN_TIME_AND_SCENARIO;
}

constexpr bool isTimeConstant(VariabilityType variability)
{
    return variability == VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
           || variability == VariabilityType::VARYING_IN_SCENARIO_ONLY;
}

constexpr bool isConstant(VariabilityType variability)
{
    return variability == VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO;
}

} // namespace Antares::Optimisation
