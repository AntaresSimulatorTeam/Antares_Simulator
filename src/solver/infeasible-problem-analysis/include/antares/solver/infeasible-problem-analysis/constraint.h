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
    hydro_production_weekly,
    short_term_storage_level,
    none
};

class Constraint
{
public:
    Constraint() = default;
    Constraint(const std::string& name, const double slackValue);

    double getSlackValue() const;

    std::size_t extractComponentsFromName();
    std::string prettyPrint() const;
    ConstraintType type() const;

private:
    std::string name_;
    std::vector<std::string> nameComponents_;
    double slackValue_;

    std::string areaName() const;
    std::string STSname() const;
    std::string timeStep() const;
    std::string shortName() const;
};
} // namespace Antares::Optimization
