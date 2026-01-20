
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
