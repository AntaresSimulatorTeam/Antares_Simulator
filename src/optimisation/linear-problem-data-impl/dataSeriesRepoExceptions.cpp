#include <string>

#include "antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h"

namespace Antares::Optimisation::LinearProblemDataImpl
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

} // namespace Antares::Optimisation::LinearProblemDataImpl
