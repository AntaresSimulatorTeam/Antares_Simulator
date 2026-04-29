// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{

void LinearProblemData::addDataSeries(std::unique_ptr<IDataSeries> dataSeries)
{
    dataSeriesRepository_.addDataSeries(std::move(dataSeries));
}

double LinearProblemData::getData(const std::string& dataSetId,
                                  LinearProblemApi::IScenario::TimeSeriesNumber timeSeriesNumber,
                                  unsigned hour) const
{
    return dataSeriesRepository_.getDataSeries(dataSetId).getData(timeSeriesNumber, hour);
}

std::span<const double> LinearProblemData::getData(const std::string& dataSetId,
                                                   unsigned timeSeriesNumber,
                                                   unsigned firstHour,
                                                   unsigned lastHour) const
{
    return dataSeriesRepository_.getDataSeries(dataSetId).getData(timeSeriesNumber,
                                                                  firstHour,
                                                                  lastHour);
}

} // namespace Antares::Optimisation::LinearProblemDataImpl
