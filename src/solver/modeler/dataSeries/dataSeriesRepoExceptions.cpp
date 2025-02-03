#include <string>

#include "antares/solver/modeler/dataSeries/dataSeriesRepo.h"

namespace Antares::Solver::Modeler::DataSeries
{
DataSeriesRepository::Empty::Empty():
    std::invalid_argument("Data series repo is empty, and somebody requests data from it")
{
}

DataSeriesRepository::DataSeriesNotExist::DataSeriesNotExist(const std::string& setId):
    std::invalid_argument("Data series repo : data series '" + setId + "' does not exist")
{
}

DataSeriesRepository::DataSeriesAlreadyExists::DataSeriesAlreadyExists(const std::string& setId):
    std::invalid_argument("Data series repo : data series '" + setId + "' already exists")
{
}

} // namespace Antares::Solver::Modeler::DataSeries
