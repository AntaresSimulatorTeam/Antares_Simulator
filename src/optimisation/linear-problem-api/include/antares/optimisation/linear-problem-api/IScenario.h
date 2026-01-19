// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include <boost/algorithm/string.hpp>

namespace Antares::Optimisation::LinearProblemApi
{

/** \brief Interface for scenarios in the linear problem API.
 * A scenario is an association of years to their respective time series numbers for a given group
 * name.
 */
class IScenario
{
public:
    using Year = unsigned;
    using TimeSeriesNumber = unsigned;

    virtual ~IScenario() = default;

    explicit IScenario(std::string group):
        group_(std::move(group))
    {
        boost::to_upper(group_); // Ensure group name is uppercase for consistency
    }

    [[nodiscard]] virtual TimeSeriesNumber getData(Year year) const = 0;

    [[nodiscard]] std::string group() const
    {
        return group_;
    }

private:
    std::string group_;
};

/** \brief Empty scenario implementation of IScenario.
 * Provide a default implementation of IScenario that returns 0 for any year for a group named
 * "empty"
 */
class EmptyScenario final: public IScenario
{
public:
    EmptyScenario():
        IScenario("empty")
    {
    }

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // No data available in an empty scenario
    }
};
} // namespace Antares::Optimisation::LinearProblemApi
