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

namespace Antares::Solver::Visitors
{

class LinearStatus
{
public:
    enum Status
    {
        CONSTANT,
        LINEAR,
        NON_LINEAR
    };

    constexpr LinearStatus() = default;

    constexpr LinearStatus(const Status& status):
        status_(status)
    {
    }

    constexpr LinearStatus(const LinearStatus& other) = default;

    constexpr LinearStatus operator*(const LinearStatus& other)
    {
        switch (other)
        {
        case LinearStatus::NON_LINEAR:
            return LinearStatus::NON_LINEAR;
        case LinearStatus::CONSTANT:
            return *this;
        case LinearStatus::LINEAR:
            if (status_ == LinearStatus::CONSTANT)
            {
                return other;
            }
            else
            {
                return LinearStatus::NON_LINEAR;
            }
        };
    }

    constexpr LinearStatus operator/(const LinearStatus& other)
    {
        switch (other)
        {
        case LinearStatus::NON_LINEAR:
        case LinearStatus::LINEAR:
            return LinearStatus::NON_LINEAR;
        case LinearStatus::CONSTANT:
            return *this;
        };
    }

    constexpr LinearStatus operator+(const LinearStatus& other)
    {
        switch (other)
        {
        case LinearStatus::NON_LINEAR:
            return LinearStatus::NON_LINEAR;
        case LinearStatus::CONSTANT:
            return *this;
        case LinearStatus::LINEAR:
            if (other == LinearStatus::CONSTANT || other == LinearStatus::LINEAR)
            {
                return other;
            }
            else
            {
                return LinearStatus::NON_LINEAR;
            }
        };
    }

    constexpr LinearStatus operator-(const LinearStatus& other)
    {
        return operator+(other);
    }

    // Conversions
    constexpr explicit operator bool() const = delete;

    constexpr operator Status() const
    {
        return status_;
    }

    // Comparisons
    constexpr bool operator==(LinearStatus a) const
    {
        return status_ == a.status_;
    }

    constexpr bool operator==(Status status) const
    {
        return status_ == status;
    }

    constexpr bool operator!=(LinearStatus a) const
    {
        return status_ != a.status_;
    }

    constexpr LinearStatus operator-() const
    {
        return *this;
    }

private:
    Status status_;
};
} // namespace Antares::Solver::Visitors
