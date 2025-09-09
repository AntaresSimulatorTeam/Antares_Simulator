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
class EmptyScenario: public IScenario
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
