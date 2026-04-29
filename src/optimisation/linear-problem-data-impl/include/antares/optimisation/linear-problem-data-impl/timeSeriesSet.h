// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "dataSeries.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
class TimeSeriesSet: public IDataSeries
{
public:
    explicit TimeSeriesSet(std::string name, unsigned height);
    explicit TimeSeriesSet(std::string name, std::vector<std::vector<double>>&& tsSet);
    void add(const std::vector<double>& ts);
    void add(std::vector<double>&& ts);
    double getData(unsigned tsNumber, unsigned hour) const override;
    [[nodiscard]] std::span<const double> getData(
      LinearProblemApi::IScenario::TimeSeriesNumber tsNumber,
      unsigned firstHour,
      unsigned lastHour) const override;

private:
    unsigned height_ = 0;
    std::vector<std::vector<double>> tsSet_;

public:
    class AddTSofWrongSize final: public std::invalid_argument
    {
    public:
        explicit AddTSofWrongSize(const std::string& name,
                                  const size_t& tsSize,
                                  const unsigned& height);
    };

    class Empty final: public std::invalid_argument
    {
    public:
        explicit Empty(const std::string& name);
    };

    class RankTooBig final: public std::invalid_argument
    {
    public:
        explicit RankTooBig(const std::string& name, unsigned rank, unsigned tsSetSize);
    };

    class HourTooBig final: public std::invalid_argument
    {
    public:
        explicit HourTooBig(const std::string& name, unsigned hour);
    };
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
