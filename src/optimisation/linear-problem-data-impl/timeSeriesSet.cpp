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

#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include <stdexcept>
#include <string>

#include "antares/exception/LoadingError.hpp"
#include "antares/exception/RuntimeError.hpp"

namespace Antares::Optimisation::LinearProblemDataImpl
{
TimeSeriesSet::TimeSeriesSet(std::string name, unsigned int height):
    IDataSeries::IDataSeries(name),
    height_(height)
{
}

TimeSeriesSet::TimeSeriesSet(std::string name, std::vector<std::vector<double>>&& tsSet):
    IDataSeries::IDataSeries(name),
    tsSet_(std::move(tsSet))
{
    if (!tsSet_.empty())
    {
        height_ = tsSet_[0].size();
    }
}

void TimeSeriesSet::add(const std::vector<double>& ts)
{
    if (ts.size() != height_)
    {
        throw AddTSofWrongSize(name(), ts.size(), height_);
    }
    tsSet_.push_back(std::move(ts));
}

void TimeSeriesSet::add(std::vector<double>&& ts)
{
    if (ts.size() != height_)
    {
        throw AddTSofWrongSize(name(), ts.size(), height_);
    }
    tsSet_.push_back(std::move(ts));
}

double TimeSeriesSet::getData(LinearProblemApi::IScenario::TimeSeriesNumber tsNumber,
                              unsigned hour) const
{
    if (tsNumber == 0)
    {
        throw Antares::Error::RuntimeError(
          "Trying to get data set at rank 0. Data sets are indexed starting at 1.");
    }
    if (tsSet_.empty())
    {
        throw Empty(name());
    }
    auto tsIndex = tsNumber - 1;
    if (tsIndex > tsSet_.size() - 1)
    {
        throw RankTooBig(name(), tsNumber, tsSet_.size());
    }

    if (hour > height_ - 1)
    {
        throw HourTooBig(name(), hour);
    }
    return tsSet_[tsIndex][hour];
}

} // namespace Antares::Optimisation::LinearProblemDataImpl
