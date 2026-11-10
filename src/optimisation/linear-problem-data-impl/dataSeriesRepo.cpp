// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h"

namespace Antares::LinearProblem::DataImpl
{
void DataSeriesRepository::addDataSeries(std::unique_ptr<IDataSeries> dataSeries)
{
    std::string name = dataSeries->name();
    if (!dataSeries_.try_emplace(name, std::move(dataSeries)).second)
    {
        throw DataSeriesAlreadyExists(name);
    }
}

IDataSeries& DataSeriesRepository::getDataSeries(const std::string& setId) const
{
    if (dataSeries_.empty())
    {
        throw Empty();
    }
    const auto it = dataSeries_.find(setId);
    if (it == dataSeries_.end())
    {
        throw DataSeriesNotExist(setId);
    }
    return *it->second;
}
} // namespace Antares::LinearProblem::DataImpl
