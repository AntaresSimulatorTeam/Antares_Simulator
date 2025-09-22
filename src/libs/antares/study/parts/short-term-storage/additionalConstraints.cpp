/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
AdditionalConstraints::AdditionalConstraints():
    timeSeries(timeseriesNumbers)
{
}

AdditionalConstraints::AdditionalConstraints(std::string name,
                                             std::string id,
                                             std::string cluster_id,
                                             std::string variable,
                                             std::string operatorType,
                                             bool enabled,
                                             std::vector<SingleAdditionalConstraint> constraints):
    name(name),
    id(id),
    cluster_id(cluster_id),
    variable(variable),
    operatorType(operatorType),
    enabled(enabled),
    constraints(constraints),
    timeSeries(timeseriesNumbers)
{
}

namespace
{
bool hasValidHours(const AdditionalConstraints& ct)
{
    return std::ranges::all_of(ct.constraints,
                               [](const auto& constraint)
                               { return constraint.hasValidHoursRange(); });
}

bool hasValidVariable(const AdditionalConstraints& ct)
{
    return ct.variable == "injection" || ct.variable == "withdrawal" || ct.variable == "netting";
}

bool hasValidOperatorType(const AdditionalConstraints& ct)
{
    return ct.operatorType == "less" || ct.operatorType == "equal" || ct.operatorType == "greater";
}
} // namespace

ValidateResult validate(const AdditionalConstraints& adc)
{
    if (adc.cluster_id.empty())
    {
        return {false, "Cluster ID is empty."};
    }

    if (!hasValidVariable(adc))
    {
        return {false, "Invalid variable type. Must be 'injection', 'withdrawal', or 'netting'."};
    }

    if (!hasValidOperatorType(adc))
    {
        return {false, "Invalid operator type. Must be 'less', 'equal', or 'greater'."};
    }

    if (!hasValidHours(adc))
    {
        return {false, "Hours sets contains invalid values. Must be between 1 and 168."};
    }

    return {true, ""};
}

bool SingleAdditionalConstraint::hasValidHoursRange() const
{
    // `hours` is a sorted set; begin() gives the smallest and prev(end()) gives the largest.
    return !hours.empty() && *hours.begin() >= 1 && *std::prev(hours.end()) <= 168;
}

std::size_t AdditionalConstraints::enabledConstraintsCount() const
{
    return enabled ? constraints.size() : 0;
}
} // namespace Antares::Data::ShortTermStorage
