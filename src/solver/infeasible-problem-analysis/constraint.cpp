/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include "antares/solver/infeasible-problem-analysis/constraint.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/regex.hpp>

namespace
{
const std::string kUnknown = "<unknown>";
}

namespace Antares::Optimization
{
Constraint::Constraint(const std::string& name):
    name_(name)
{
}

void Constraint::extractComponentsFromName()
{
    boost::algorithm::split_regex(nameComponents_, name_, boost::regex("::"));
}

class StringIsNotWellFormated: public std::runtime_error
{
public:
    StringIsNotWellFormated(const std::string& error_message):
        std::runtime_error(error_message)
    {
    }
};

std::string StringBetweenAngleBrackets(const std::string& constraintName)
{
    std::vector<std::string> split_name;
    boost::split(split_name, constraintName, boost::is_any_of("<>"));

    std::string err_msg = "Error: ";
    if (split_name.size() < 3)
    {
        err_msg += "constraint name '" + constraintName + "' misses '<' and/or '>' bracket";
        throw StringIsNotWellFormated(err_msg);
    }
    if (split_name[1].empty())
    {
        err_msg += "constraint name '" + constraintName + "' must be of format '*<str>*'";
        throw StringIsNotWellFormated(err_msg);
    }
    return split_name[1];
}

std::string Constraint::areaName() const
{
    return StringBetweenAngleBrackets(nameComponents_.at(1));
}

std::string Constraint::timeStep() const
{
    return StringBetweenAngleBrackets(nameComponents_.at(nameComponents_.size() - 2));
}

ConstraintType Constraint::type() const
{
    assert(nameComponents_.size() > 1);
    if (nameComponents_.at(1) == "hourly")
    {
        return ConstraintType::binding_constraint_hourly;
    }
    if (nameComponents_.at(1) == "daily")
    {
        return ConstraintType::binding_constraint_daily;
    }
    if (nameComponents_.at(1) == "weekly")
    {
        return ConstraintType::binding_constraint_weekly;
    }
    if (nameComponents_.at(0) == "FictiveLoads")
    {
        return ConstraintType::fictitious_load;
    }
    if (nameComponents_.at(0) == "AreaHydroLevel")
    {
        return ConstraintType::hydro_reservoir_level;
    }
    if (nameComponents_.at(0) == "HydroPower")
    {
        return ConstraintType::hydro_production_weekly;
    }
    if (nameComponents_.at(0) == "Level")
    {
        return ConstraintType::short_term_storage_level;
    }
    return ConstraintType::none;
}

std::string Constraint::shortName() const
{
    return nameComponents_.at(0);
}

std::string Constraint::STSname() const
{
    return StringBetweenAngleBrackets(nameComponents_.at(2));
}

std::string Constraint::prettyPrint() const
{
    switch (type())
    {
    case ConstraintType::binding_constraint_hourly:
        return "Hourly binding constraint '" + shortName() + "' at hour " + timeStep();
    case ConstraintType::binding_constraint_daily:
        return "Daily binding constraint '" + shortName() + "' at day " + timeStep();
    case ConstraintType::binding_constraint_weekly:
        return "Weekly binding constraint '" + shortName();
    case ConstraintType::fictitious_load:
        return "Last resort shedding status at area '" + areaName() + "' at hour " + timeStep();
    case ConstraintType::hydro_reservoir_level:
        return "Hydro reservoir constraint at area '" + areaName() + "' at hour " + timeStep();
    case ConstraintType::hydro_production_weekly:
        return "Hydro weekly production at area '" + areaName() + "'";
    case ConstraintType::short_term_storage_level:
        return "Short-term-storage reservoir constraint at area '" + areaName() + "' in STS '"
               + STSname() + "' at hour " + timeStep();
    default:
        return kUnknown;
    }
}
} // namespace Antares::Optimization
