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
