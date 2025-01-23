#include "antares/solver/modeler/dataSeries/dataSeriesRepo.h"

#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{
void DataSeriesRepository::addDataSeries(std::unique_ptr<IDataSeries> dataSeries)
{
    std::string name = dataSeries->name();
    dataSeries_[name] = std::move(dataSeries);
}

IDataSeries& DataSeriesRepository::getDataSeries(const std::string& setId)
{
    std::string error_message = err_prefix;
    if (dataSeries_.empty())
    {
        error_message += "empty";
        throw std::invalid_argument(error_message);
    }
    if (!dataSeries_.contains(setId))
    {
        error_message += "data series '" + setId + "' does not exist";
        throw std::invalid_argument(error_message);
    }
    return *(dataSeries_[setId]);
}
} // namespace Antares::Solver::Modeler::DataSeries
