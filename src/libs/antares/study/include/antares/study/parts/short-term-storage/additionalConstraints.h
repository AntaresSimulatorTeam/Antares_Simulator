/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <set>
#include <string>
#include <vector>

#include "antares/series/series.h"

namespace Antares::Data::ShortTermStorage
{

class SingleAdditionalConstraint
{
public:
    std::set<int> hours;
    unsigned int globalIndex = 0;
    unsigned int localIndex = 0;
    bool hasValidHoursRange() const;
};

struct ValidateResult

{
    bool ok;
    std::string error_msg;
};

class AdditionalConstraints
{
public:
    AdditionalConstraints();
    AdditionalConstraints(std::string name,
                          std::string id,
                          std::string cluster_id,
                          std::string variable,
                          std::string operatorType,
                          bool enabled,
                          std::vector<SingleAdditionalConstraint> constraints);

    AdditionalConstraints(const AdditionalConstraints&) = delete;
    AdditionalConstraints& operator=(const AdditionalConstraints&) = delete;

    AdditionalConstraints(AdditionalConstraints&& other) noexcept = delete;
    AdditionalConstraints& operator=(AdditionalConstraints&& other) noexcept = delete;

    ~AdditionalConstraints() = default;

    std::string name;
    std::string id;
    std::string cluster_id;
    std::string variable;
    std::string operatorType;
    bool enabled = true;
    std::vector<SingleAdditionalConstraint> constraints;

    // Number of enabled constraints
    std::size_t enabledConstraintsCount() const;

    TimeSeries& rhs()
    {
        return timeSeries;
    }

    const TimeSeries& rhs() const
    {
        return timeSeries;
    }

    TimeSeriesNumbers timeseriesNumbers;
    TimeSeries timeSeries;
};

ValidateResult validate(const AdditionalConstraints&);

} // namespace Antares::Data::ShortTermStorage
