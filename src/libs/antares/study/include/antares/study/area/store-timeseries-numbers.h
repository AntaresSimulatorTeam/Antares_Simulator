// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/writer/i_writer.h>

#include "area.h"

namespace Antares::Data
{
template<TimeSeriesType TimeSeriesT>
void singleAreaStoreTimeseriesNumbers(Solver::IResultWriter& writer, const Area& area);

template<TimeSeriesType TimeSeriesT>
void storeTimeseriesNumbers(Solver::IResultWriter& writer, const AreaList& area)
{
    // Each area in the list
    area.each([&writer](const Area& a)
              { singleAreaStoreTimeseriesNumbers<TimeSeriesT>(writer, a); });
}

} // namespace Antares::Data
