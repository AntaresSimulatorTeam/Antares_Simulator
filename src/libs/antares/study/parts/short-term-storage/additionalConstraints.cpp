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

#include "antares/study/parts/short-term-storage/additionalConstraints.h"

#include <algorithm>

namespace Antares::Data::ShortTermStorage
{
AdditionalConstraints::ValidateResult AdditionalConstraints::validate() const
{
    if (cluster_id.empty())
    {
        return {false, "Cluster ID is empty."};
    }

    if (!isValidVariable())
    {
        return {false, "Invalid variable type. Must be 'injection', 'withdrawal', or 'netting'."};
    }

    if (!isValidOperatorType())
    {
        return {false, "Invalid operator type. Must be 'less', 'equal', or 'greater'."};
    }

    if (!isValidHours())
    {
        return {false, "Hours sets contains invalid values. Must be between 1 and 168."};
    }

    return {true, ""};
}

bool SingleAdditionalConstraint::isValidHoursRange() const
{
    // `hours` is a sorted set; begin() gives the smallest and prev(end()) gives the largest.
    return !hours.empty() && *hours.begin() >= 1 && *std::prev(hours.end()) <= 168;
}

bool AdditionalConstraints::isValidHours() const
{
    return std::ranges::all_of(constraints,
                               [](const auto& constraint)
                               { return constraint.isValidHoursRange(); });
}

bool AdditionalConstraints::isValidVariable() const
{
    return variable == "injection" || variable == "withdrawal" || variable == "netting";
}

bool AdditionalConstraints::isValidOperatorType() const
{
    return operatorType == "less" || operatorType == "equal" || operatorType == "greater";
}

std::size_t AdditionalConstraints::enabledConstraints() const
{
    return enabled ? constraints.size() : 0;
}
} // namespace Antares::Data::ShortTermStorage
