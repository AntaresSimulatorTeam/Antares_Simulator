// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_SIMULATION_GENERATE_TIMESERIES_H__
#define __SOLVER_SIMULATION_GENERATE_TIMESERIES_H__

#include <antares/study/study.h>

#include "ITimeSeriesNumbersWriter.h"

namespace Antares::Solver::TimeSeriesNumbers
{
bool CheckNumberOfColumns(const Data::AreaList& areas);

/*!
** \brief Generate all time-series numbers for a given set
*/
bool Generate(Data::Study& study);

void StoreTimeSeriesNumbersIntoOuput(Data::Study& study, IResultWriter& resultWriter);

} // namespace Antares::Solver::TimeSeriesNumbers

#endif // __SOLVER_SIMULATION_GENERATE_TIMESERIES_H__
