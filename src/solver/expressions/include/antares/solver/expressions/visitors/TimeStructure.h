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
enum class TIME_STRUCTURE_TYPE : int
{
    CONSTANT = 0,
    VARYING_IN_TIME_ONLY = 1,
    VARYING_IN_SCENARIO_ONLY = 2,
    BOTH = 3
};

inline TIME_STRUCTURE_TYPE operator|(const TIME_STRUCTURE_TYPE& left,
                                     const TIME_STRUCTURE_TYPE& right)
{
    return static_cast<TIME_STRUCTURE_TYPE>(static_cast<int>(left) | static_cast<int>(right));
}

// better name?
/*class ExpressionTimeType
{
public:
    ExpressionTimeType() = default;
    ExpressionTimeType(const ExpressionTimeType& other) = default;

    constexpr ExpressionTimeType(const Nodes::TimeIndex& timeIndex):
        timeIndex_(timeIndex)
    {
    }

    [[nodiscard]] ExpressionTimeType Connect(const Nodes::TimeIndex& other) const
    {
        bool isTimeVarying = other.IsTimeVarying() || timeIndex_.IsTimeVarying();
        bool isScenarioVarying = other.IsScenarioVarying() || timeIndex_.IsScenarioVarying();
        return ExpressionTimeType(Nodes::TimeIndex(isTimeVarying, isScenarioVarying));
    }

    ExpressionTimeType operator*(const ExpressionTimeType& other) const

    {
        return Connect(other);
    }

    ExpressionTimeType operator/(const ExpressionTimeType& other) const

    {
        return Connect(other);
    }

    ExpressionTimeType operator+(const ExpressionTimeType& other) const
    {
        return Connect(other);
    }

    ExpressionTimeType operator-(const ExpressionTimeType& other) const
    {
        return Connect(other);
    }

    // Conversions
    constexpr explicit operator bool() const = delete;

    operator Nodes::TimeIndex() const
    {
        return timeIndex_;
    }

    Nodes::TimeIndex GetTimeIndex() const
    {
        return timeIndex_;
    }

    // Comparisons
    bool operator==(ExpressionTimeType other) const
    {
        return timeIndex_ == other.timeIndex_;
    }

    bool operator==(Nodes::TimeIndex timeIndex) const
    {
        return timeIndex_ == timeIndex;
    }

    ExpressionTimeType operator-() const
    {
        return *this;
    }

private:
    Nodes::TimeIndex timeIndex_;
};*/
} // namespace Antares::Solver::Visitors
