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

    AdditionalConstraints(const AdditionalConstraints& other):
        name(other.name),
        cluster_id(other.cluster_id),
        variable(other.variable),
        operatorType(other.operatorType),
        enabled(other.enabled),
        rhs(other.rhs),
        constraints(other.constraints),
        tsNumbers(other.tsNumbers),
        series(tsNumbers)
    {
        series.timeSeries = other.series.timeSeries;
    }

    AdditionalConstraints(AdditionalConstraints&& other) noexcept:
        name(std::move(other.name)),
        cluster_id(std::move(other.cluster_id)),
        variable(std::move(other.variable)),
        operatorType(std::move(other.operatorType)),
        enabled(other.enabled),
        rhs(std::move(other.rhs)),
        constraints(std::move(other.constraints)),
        tsNumbers(std::move(other.tsNumbers)), // if movable
        series(tsNumbers)                      // must rebind series to our tsNumbers
    {
        series.timeSeries = std::move(other.series.timeSeries);
    }

    AdditionalConstraints& operator=(const AdditionalConstraints& other)
    {
        if (this != &other)
        {
            name = other.name;
            cluster_id = other.cluster_id;
            variable = other.variable;
            operatorType = other.operatorType;
            enabled = other.enabled;
            rhs = other.rhs;
            constraints = other.constraints;
            tsNumbers = other.tsNumbers;
            series.timeSeries = other.series.timeSeries;
        }
        return *this;
    }

    AdditionalConstraints& operator=(AdditionalConstraints&& other) noexcept
    {
        if (this != &other)
        {
            name = std::move(other.name);
            cluster_id = std::move(other.cluster_id);
            variable = std::move(other.variable);
            operatorType = std::move(other.operatorType);
            enabled = other.enabled;
            rhs = std::move(other.rhs);
            constraints = std::move(other.constraints);
            tsNumbers = std::move(other.tsNumbers);

            series.timeSeries = std::move(other.series.timeSeries);
        }
        return *this;
    }

    std::string name;
    std::string cluster_id;
    std::string variable;
    std::string operatorType;
    bool enabled = true;
    std::vector<double> rhs;

    std::vector<SingleAdditionalConstraint> constraints;

    struct ValidateResult
    {
        bool ok;
        std::string error_msg;
    };

    // Number of enabled constraints
    std::size_t enabledConstraints() const;

    ValidateResult validate() const;

    TimeSeriesNumbers tsNumbers;
    /*! Data for rhs time-series */
    TimeSeries series;

private:
    bool isValidVariable() const;
    bool isValidOperatorType() const;

    bool isValidHours() const;
};
} // namespace Antares::Data::ShortTermStorage
