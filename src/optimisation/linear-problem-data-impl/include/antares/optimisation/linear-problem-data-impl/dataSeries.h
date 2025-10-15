
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

#include <span>
#include <string>

#include <antares/optimisation/linear-problem-api/IScenario.h>

namespace Antares::Optimisation::LinearProblemDataImpl
{

class IDataSeries
{
public:
    virtual ~IDataSeries() = default;

    explicit IDataSeries(std::string name):
        name_(std::move(name))
    {
    }

    [[nodiscard]] virtual double getData(
      LinearProblemApi::IScenario::TimeSeriesNumber time_series_number,
      unsigned int hour) const
      = 0;

    [[nodiscard]] virtual std::span<const double> getData(
      LinearProblemApi::IScenario::TimeSeriesNumber time_series_number,
      unsigned firstHour,
      unsigned lastHour) const
      = 0;

    [[nodiscard]] std::string name() const
    {
        return name_;
    }

private:
    std::string name_;
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
