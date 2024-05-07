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
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimization
{
enum class ConstraintType
{
    binding_constraint_hourly,
    binding_constraint_daily,
    binding_constraint_weekly,
    fictitious_load,
    hydro_reservoir_level,
    short_term_storage_level,
    none
};

class Constraint
{
public:
    // Construct object
    Constraint() = default;
    Constraint(const std::string& input, const double slackValue);

    // Raw members
    double getSlackValue() const;

    // Extract items, check consistency
    std::size_t extractItems();
    std::string prettyPrint() const;
    ConstraintType getType() const;

private:
    std::string mInput;
    std::vector<std::string> mItems;
    double mSlackValue;

    // Get specific items
    std::string getAreaName() const;
    std::string getSTSName() const;
    std::string getTimeStepInYear() const;
    std::string getBindingConstraintName() const;
};
} // namespace Antares::Optimization
