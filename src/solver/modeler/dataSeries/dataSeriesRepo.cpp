#include "antares/solver/modeler/dataSeries/dataSeriesRepo.h"

#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{
void DataSeriesRepository::addDataSeries(std::unique_ptr<IDataSeries> dataSeries)
{
    dataSeries_[dataSeries->name()] = std::move(dataSeries);
}

IDataSeries& DataSeriesRepository::getDataSeries(std::string setId)
{
    if (!dataSeries_.contains(setId))
    {
        std::string error_message = "Data series repo : data series named '" + setId
                                    + "' does not exist.";
        throw std::invalid_argument(error_message);
    }
    return *(dataSeries_[setId]);
}
} // namespace Antares::Solver::Modeler::DataSeries
