#include "antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
void DataSeriesRepository::addDataSeries(std::unique_ptr<IDataSeries> dataSeries)
{
    std::string name = dataSeries->name();
    if (dataSeries_.contains(name))
    {
        throw DataSeriesAlreadyExists(name);
    }
    dataSeries_[name] = std::move(dataSeries);
}

IDataSeries& DataSeriesRepository::getDataSeries(const std::string& setId)
{
    if (dataSeries_.empty())
    {
        throw Empty();
    }
    if (!dataSeries_.contains(setId))
    {
        throw DataSeriesNotExist(setId);
    }
    return *(dataSeries_[setId]);
}
} // namespace Antares::Optimisation::LinearProblemDataImpl
