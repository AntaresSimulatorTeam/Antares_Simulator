// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
