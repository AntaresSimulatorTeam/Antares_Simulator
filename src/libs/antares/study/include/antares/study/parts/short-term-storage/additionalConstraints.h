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
    bool isValidHoursRange() const;
};

class AdditionalConstraints
{
public:
    AdditionalConstraints();
    AdditionalConstraints(std::string name,
                          std::string cluster_id,
                          std::string variable,
                          std::string operatorType,
                          bool enabled,
                          std::vector<SingleAdditionalConstraint> constraints);

    AdditionalConstraints(const AdditionalConstraints& other) = default;
    AdditionalConstraints(AdditionalConstraints&& other) noexcept = default;
    AdditionalConstraints& operator=(const AdditionalConstraints& other) = default;
    AdditionalConstraints& operator=(AdditionalConstraints&& other) noexcept = default;

    std::string name;
    std::string cluster_id;
    std::string variable;
    std::string operatorType;
    bool enabled = true;
    std::vector<SingleAdditionalConstraint> constraints;

    struct ValidateResult

    {
        bool ok;
        std::string error_msg;
    };

    // Number of enabled constraints
    std::size_t enabledConstraints() const;

    ValidateResult validate() const;

    TimeSeries& rhs()
    {
        return rhs_;
    }

    const TimeSeries& rhs() const
    {
        return rhs_;
    }

private:
    TimeSeriesNumbers tsNumbers;
    TimeSeries rhs_; ///< contains both tsNumbers and series
    bool isValidVariable() const;
    bool isValidOperatorType() const;

    bool isValidHours() const;
};
} // namespace Antares::Data::ShortTermStorage
