#include "antares/solver/modeler/dataSeries/dataSeriesRepoExceptions.h"

#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
DataSeriesRepo_Empty::DataSeriesRepo_Empty():
    std::invalid_argument("Data series repo is empty, and somebody requests data from it")
{
}

DataSeriesRepo_DataSeriesNotExist::DataSeriesRepo_DataSeriesNotExist(const std::string setId):
    std::invalid_argument("Data series repo : data series '" + setId + "' does not exist")
{
}

DataSeriesRepo_DSalreadyExists::DataSeriesRepo_DSalreadyExists(const std::string setId):
    std::invalid_argument("Data series repo : data series '" + setId + "' already exists")
{
}

} // namespace Antares::Solver::Modeler::DataSeries
