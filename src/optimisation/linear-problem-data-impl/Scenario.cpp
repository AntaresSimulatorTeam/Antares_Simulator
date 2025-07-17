// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include "include/antares/optimisation/linear-problem-data-impl/Scenario.h"

#include "antares/exception/RuntimeError.hpp"

#include "fmt/format.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
LinearProblemApi::IScenario::TimeSeriesNumber Scenario::getData(Year year) const
{
    auto it = timeSerieData_.find(year);
    if (it == timeSerieData_.end())
    {
        throw Error::RuntimeError(
          fmt::format("In scenario group '{}', time serie for year {} does not exist.",
                      group(),
                      year));
    }
    return it->second;
}

void Scenario::setTimeSerieNumber(Year year, TimeSeriesNumber timeSeriesNumber)
{
    if (timeSerieData_.contains(year))
    {
        throw Error::RuntimeError(
          fmt::format("Time serie number for year {} already exists.", year));
    }
    timeSerieData_[year] = timeSeriesNumber;
}
} // namespace Antares::Optimisation::LinearProblemDataImpl
