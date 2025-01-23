#include "antares/solver/modeler/dataSeries/dataSeriesRepo.h"

#include <stdexcept>

#include "antares/solver/modeler/dataSeries/dataSeriesRepoExceptions.h"

namespace Antares::Solver::Modeler::DataSeries
{
void DataSeriesRepository::addDataSeries(std::unique_ptr<IDataSeries> dataSeries)
{
    std::string name = dataSeries->name();
    if (dataSeries_.contains(name))
    {
        throw DataSeriesRepo_DSalreadyExists(name);
    }
    dataSeries_[name] = std::move(dataSeries);
}

IDataSeries& DataSeriesRepository::getDataSeries(const std::string& setId)
{
    if (dataSeries_.empty())
    {
        throw DataSeriesRepo_Empty();
    }
    if (!dataSeries_.contains(setId))
    {
        throw DataSeriesRepo_DataSeriesNotExist(setId);
    }
    return *(dataSeries_[setId]);
}
} // namespace Antares::Solver::Modeler::DataSeries
