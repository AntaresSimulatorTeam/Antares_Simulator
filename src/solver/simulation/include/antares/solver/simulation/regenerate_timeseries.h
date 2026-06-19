// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/benchmarking/DurationCollector.h>
#include <antares/study/study.h>
#include <antares/writer/i_writer.h>

namespace Antares::Solver::Simulation
{
void regenerateTimeSeries(Data::Study& study,
                          IResultWriter& resultWriter,
                          Benchmarking::DurationCollector& durationCollector);

}
