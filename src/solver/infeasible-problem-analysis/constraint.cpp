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
#include "constraint.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>

namespace
{
const std::string kUnknown = "<unknown>";
}

namespace Antares::Optimization
{
Constraint::Constraint(const std::string& input, const double slackValue):
    mInput(input),
    mSlackValue(slackValue)
{
}

std::size_t Constraint::extractItems()
{
    const auto beg = mInput.begin();
    const auto end = mInput.end();
    std::size_t newPos = 0;
    const std::size_t sepSize = 2;
    const std::size_t inputSize = mInput.size();
    for (std::size_t pos = 0; pos < inputSize; pos = newPos + sepSize)
    {
        newPos = mInput.find("::", pos);
        if (newPos == std::string::npos)
        {
            mItems.emplace_back(beg + pos, end);
            break;
        }
        if (newPos > pos)
        {
            mItems.emplace_back(beg + pos, beg + newPos);
        }
    }
    return mItems.size();
}

double Constraint::getSlackValue() const
{
    return mSlackValue;
}

class StringIsNotWellFormated: public std::runtime_error
{
public:
    StringIsNotWellFormated(const std::string& error_message):
        std::runtime_error(error_message)
    {
    }
};

std::string StringBetweenAngleBrackets(const std::string& str)
{
    const auto& begin = str.begin();
    const auto& end = str.end();

    auto left = std::find(begin, end, '<');

    if (left == end)
    {
        std::ostringstream stream;
        stream << std::string("Error the string: ") << std::quoted(str)
               << " does not contains the left angle bracket " << std::quoted("<");
        throw StringIsNotWellFormated(stream.str());
    }

    auto right = std::find(begin, end, '>');
    if (right == end)
    {
        std::ostringstream stream;
        stream << std::string("Error the string: ") << std::quoted(str)
               << " does not contains the right angle bracket " << std::quoted(">");
        throw StringIsNotWellFormated(stream.str());
    }

    if (std::distance(left, right) <= 1)
    {
        std::ostringstream stream;
        stream << std::string("Error the string: ") << std::quoted(str) << " must be of format  "
               << std::quoted("*<str>*");
        throw StringIsNotWellFormated(stream.str());
    }
    return std::string(left + 1, right);
}

std::string Constraint::getAreaName() const
{
    if ((getType() == ConstraintType::binding_constraint_hourly)
        || (getType() == ConstraintType::binding_constraint_daily)
        || (getType() == ConstraintType::binding_constraint_weekly))
    {
        return "<none>";
    }
    return StringBetweenAngleBrackets(mItems.at(1));
}

std::string Constraint::getTimeStepInYear() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
    case ConstraintType::binding_constraint_daily:
    case ConstraintType::fictitious_load:
    case ConstraintType::hydro_reservoir_level:
    case ConstraintType::short_term_storage_level:
        return StringBetweenAngleBrackets(mItems.at(mItems.size() - 2));
    default:
        return kUnknown;
    }
}

ConstraintType Constraint::getType() const
{
    assert(mItems.size() > 1);
    if (mItems.at(1) == "hourly")
    {
        return ConstraintType::binding_constraint_hourly;
    }
    if (mItems.at(1) == "daily")
    {
        return ConstraintType::binding_constraint_daily;
    }
    if (mItems.at(1) == "weekly")
    {
        return ConstraintType::binding_constraint_weekly;
    }
    if (mItems.at(0) == "FictiveLoads")
    {
        return ConstraintType::fictitious_load;
    }
    if (mItems.at(0) == "AreaHydroLevel")
    {
        return ConstraintType::hydro_reservoir_level;
    }
    if (mItems.at(0) == "Level")
    {
        return ConstraintType::short_term_storage_level;
    }
    return ConstraintType::none;
}

std::string Constraint::getBindingConstraintName() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
    case ConstraintType::binding_constraint_daily:
    case ConstraintType::binding_constraint_weekly:
        return mItems.at(0);
    default:
        return kUnknown;
    }
}

std::string Constraint::getSTSName() const
{
    if (getType() == ConstraintType::short_term_storage_level)
    {
        return StringBetweenAngleBrackets(mItems.at(2));
    }
    else
    {
        return kUnknown;
    }
}

std::string Constraint::prettyPrint() const
{
    switch (getType())
    {
    case ConstraintType::binding_constraint_hourly:
        return "Hourly binding constraint '" + getBindingConstraintName() + "' at hour "
               + getTimeStepInYear();
    case ConstraintType::binding_constraint_daily:
        return "Daily binding constraint '" + getBindingConstraintName() + "' at day "
               + getTimeStepInYear();
    case ConstraintType::binding_constraint_weekly:
        return "Weekly binding constraint '" + getBindingConstraintName();

    case ConstraintType::fictitious_load:
        return "Last resort shedding status at area '" + getAreaName() + "' at hour "
               + getTimeStepInYear();
    case ConstraintType::hydro_reservoir_level:
        return "Hydro reservoir constraint at area '" + getAreaName() + "' at hour "
               + getTimeStepInYear();
    case ConstraintType::short_term_storage_level:
        return "Short-term-storage reservoir constraint at area '" + getAreaName() + "' in STS '"
               + getSTSName() + "' at hour " + getTimeStepInYear();

    default:
        return kUnknown;
    }
}
} // namespace Antares::Optimization
