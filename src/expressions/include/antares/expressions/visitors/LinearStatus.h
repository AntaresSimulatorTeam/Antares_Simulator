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

namespace Antares::Expressions::Visitors
{
/**
 * @brief Represents the linearity of a node.
 */
enum class LinearStatus : char
{
    CONSTANT = 0,
    LINEAR = 1,
    NON_LINEAR = 2
};

/**
 * @brief Combines two LinearStatus values into a single character.
 *
 * @param a The first LinearStatus value.
 * @param b The second LinearStatus value.
 *
 * @return The combined LinearStatus value as a character.
 */
constexpr char pair(LinearStatus a, LinearStatus b)
{
    return static_cast<char>(a) << 4 | static_cast<char>(b);
}

/**
 * @brief Multiplies two LinearStatus values.
 *
 * @param a The first LinearStatus value.
 * @param b The second LinearStatus value.
 *
 * @return The resulting LinearStatus value based on the multiplication of a and b.
 */
constexpr LinearStatus operator*(LinearStatus a, LinearStatus b)
{
    switch (pair(a, b))
    {
    case pair(LinearStatus::CONSTANT, LinearStatus::CONSTANT):
        return LinearStatus::CONSTANT;
    case pair(LinearStatus::CONSTANT, LinearStatus::LINEAR):
        return LinearStatus::LINEAR;
    case pair(LinearStatus::CONSTANT, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    case pair(LinearStatus::LINEAR, LinearStatus::CONSTANT):
        return LinearStatus::LINEAR;
    case pair(LinearStatus::LINEAR, LinearStatus::LINEAR):
    case pair(LinearStatus::LINEAR, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    case pair(LinearStatus::NON_LINEAR, LinearStatus::CONSTANT):
    case pair(LinearStatus::NON_LINEAR, LinearStatus::LINEAR):
    case pair(LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    default:
        return LinearStatus::NON_LINEAR;
    }
}

/**
 * @brief Divides two LinearStatus values.
 *
 * @param a The first LinearStatus value.
 * @param b The second LinearStatus value.
 *
 * @return The resulting LinearStatus value based on the division of a and b.
 */
constexpr LinearStatus operator/(LinearStatus a, LinearStatus b)
{
    switch (pair(a, b))
    {
    case pair(LinearStatus::CONSTANT, LinearStatus::CONSTANT):
        return LinearStatus::CONSTANT;
    case pair(LinearStatus::CONSTANT, LinearStatus::LINEAR):
        return LinearStatus::NON_LINEAR;
    case pair(LinearStatus::CONSTANT, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    case pair(LinearStatus::LINEAR, LinearStatus::CONSTANT):
        return LinearStatus::LINEAR;
    case pair(LinearStatus::LINEAR, LinearStatus::LINEAR):
    case pair(LinearStatus::LINEAR, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    case pair(LinearStatus::NON_LINEAR, LinearStatus::CONSTANT):
    case pair(LinearStatus::NON_LINEAR, LinearStatus::LINEAR):
    case pair(LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    default:
        return LinearStatus::NON_LINEAR;
    }
}

/**
 * @brief Add two LinearStatus values.
 *
 * @param a The first LinearStatus value.
 * @param b The second LinearStatus value.
 *
 * @return The resulting LinearStatus value based on the addition of a and b.
 */
constexpr LinearStatus operator+(LinearStatus a, LinearStatus b)
{
    switch (pair(a, b))
    {
    case pair(LinearStatus::CONSTANT, LinearStatus::CONSTANT):
        return LinearStatus::CONSTANT;
    case pair(LinearStatus::CONSTANT, LinearStatus::LINEAR):
        return LinearStatus::LINEAR;
    case pair(LinearStatus::CONSTANT, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    case pair(LinearStatus::LINEAR, LinearStatus::CONSTANT):
        return LinearStatus::LINEAR;
    case pair(LinearStatus::LINEAR, LinearStatus::LINEAR):
        return LinearStatus::LINEAR;
    case pair(LinearStatus::LINEAR, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    case pair(LinearStatus::NON_LINEAR, LinearStatus::CONSTANT):
    case pair(LinearStatus::NON_LINEAR, LinearStatus::LINEAR):
    case pair(LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR):
        return LinearStatus::NON_LINEAR;

    default:
        return LinearStatus::NON_LINEAR;
    }
}

/**
 * @brief Subtracts two LinearStatus values.
 *
 * @param a The first LinearStatus value.
 * @param b The second LinearStatus value.
 *
 * @return The resulting LinearStatus value based on the subtraction of a and b.
 */
constexpr LinearStatus operator-(LinearStatus a, LinearStatus b)
{
    return operator+(a, b);
}

/**
 * @brief Negates a LinearStatus value (no effect).
 *
 * @param a The LinearStatus value to negate.
 *
 * @return The unchanged LinearStatus value.
 */
constexpr LinearStatus operator-(LinearStatus a)
{
    return a;
}
} // namespace Antares::Expressions::Visitors
