// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>
#include <string>

#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{

TimeSeriesSet::AddTSofWrongSize::AddTSofWrongSize(const std::string& name,
                                                  const size_t& tsSize,
                                                  const unsigned& height):
    std::invalid_argument(
      fmt::format("TS set '{}' : add a TS of size {} in a set of height {}", name, tsSize, height))
{
}

TimeSeriesSet::Empty::Empty(const std::string& name):
    std::invalid_argument(
      fmt::format("TS set '{}' : empty, requesting a value makes no sense", name))
{
}

TimeSeriesSet::RankTooBig::RankTooBig(const std::string& name, unsigned rank, unsigned tsSetSize):
    std::invalid_argument(
      fmt::format("TS set '{}' : TS number {} exceeds TS set's number of columns ({})",
                  name,
                  rank,
                  tsSetSize))
{
}

TimeSeriesSet::HourTooBig::HourTooBig(const std::string& name, unsigned int hour):
    std::invalid_argument(fmt::format("TS set '{}' : hour {} exceeds TS set's height", name, hour))
{
}

} // namespace Antares::Optimisation::LinearProblemDataImpl
