// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
