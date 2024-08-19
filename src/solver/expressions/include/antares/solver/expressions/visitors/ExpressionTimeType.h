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
#include <antares/solver/expressions/nodes/TimeIndex.h>

namespace Antares::Solver::Visitors
{

// better name?
class ExpressionTimeType
{
public:
    ExpressionTimeType() = default;
    ExpressionTimeType(const ExpressionTimeType& other) = default;

    constexpr ExpressionTimeType(const Nodes::TimeType& timeType):
        timeType_(timeType)
    {
    }

    constexpr ExpressionTimeType Connect(const Nodes::TimeType& other)
    {
        switch (other)
        {
        case Nodes::TimeType::TIME_DEPENDANT_AND_SCENARIZED:
            return Nodes::TimeType::TIME_DEPENDANT_AND_SCENARIZED;
        case Nodes::TimeType::SCENARIZED_ONLY:
            if (timeType_ == Nodes::TimeType::TIME_DEPENDANT_AND_SCENARIZED
                || timeType_ == Nodes::TimeType::TIME_DEPENDANT_ONLY)
            {
                return Nodes::TimeType::TIME_DEPENDANT_AND_SCENARIZED;
            }
            else
            {
                return Nodes::TimeType::SCENARIZED_ONLY;
            }
        case Nodes::TimeType::TIME_DEPENDANT_ONLY:

            if (timeType_ == Nodes::TimeType::TIME_DEPENDANT_AND_SCENARIZED
                || timeType_ == Nodes::TimeType::SCENARIZED_ONLY)
            {
                return Nodes::TimeType::TIME_DEPENDANT_AND_SCENARIZED;
            }
            else
            {
                return Nodes::TimeType::TIME_DEPENDANT_ONLY;
            }
        case Nodes::TimeType::CONSTANT:
            return timeType_;
        default:
            return Nodes::TimeType::CONSTANT;
        }
    }

    constexpr ExpressionTimeType operator*(const ExpressionTimeType& other)

    {
        return Connect(other);
    }

    constexpr ExpressionTimeType operator/(const ExpressionTimeType& other)

    {
        return Connect(other);
    }

    constexpr ExpressionTimeType operator+(const ExpressionTimeType& other)
    {
        return Connect(other);
    }

    constexpr ExpressionTimeType operator-(const ExpressionTimeType& other)
    {
        return Connect(other);
    }

    // Conversions
    constexpr explicit operator bool() const = delete;

    constexpr operator Nodes::TimeType() const
    {
        return timeType_;
    }

    // Comparisons
    constexpr bool operator==(ExpressionTimeType other) const
    {
        return timeType_ == other.timeType_;
    }

    constexpr bool operator==(Nodes::TimeType timeType) const
    {
        return timeType_ == timeType;
    }

    constexpr bool operator!=(ExpressionTimeType other) const
    {
        return timeType_ != other.timeType_;
    }

    constexpr ExpressionTimeType operator-() const
    {
        return *this;
    }

private:
    Nodes::TimeType timeType_;
};
} // namespace Antares::Solver::Visitors
