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

#include <ostream>

namespace Antares::Solver::Visitors
{

enum class LinearStatus
{
    CONSTANT,
    LINEAR,
    NON_LINEAR
};

constexpr LinearStatus operator*(LinearStatus a, LinearStatus b)
{
    switch (b)
    {
    case LinearStatus::NON_LINEAR:
        return LinearStatus::NON_LINEAR;
    case LinearStatus::CONSTANT:
        return a;
    case LinearStatus::LINEAR:
        if (a == LinearStatus::CONSTANT)
        {
            return b;
        }
        else
        {
            return LinearStatus::NON_LINEAR;
        }
    default:
        return LinearStatus::NON_LINEAR;
    }
}

constexpr LinearStatus operator/(LinearStatus a, LinearStatus b)
{
    switch (b)
    {
    case LinearStatus::NON_LINEAR:
    case LinearStatus::LINEAR:
        return LinearStatus::NON_LINEAR;
    case LinearStatus::CONSTANT:
        return a;
    default:
        return LinearStatus::NON_LINEAR;
    }
}

constexpr LinearStatus operator+(LinearStatus a, LinearStatus b)
{
    switch (b)
    {
    case LinearStatus::NON_LINEAR:
        return LinearStatus::NON_LINEAR;
    case LinearStatus::CONSTANT:
        return a;
    case LinearStatus::LINEAR:
        return b;

    default:
        return LinearStatus::NON_LINEAR;
    }
}

constexpr LinearStatus operator-(LinearStatus a, LinearStatus b)
{
    return operator+(a, b);
}

constexpr LinearStatus operator-(LinearStatus a)
{
    return a;
}

std::ostream& operator<<(std::ostream& os, LinearStatus s);

} // namespace Antares::Solver::Visitors
