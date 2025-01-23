#pragma once
#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{
class DataSeriesRepo_Empty: public std::invalid_argument
{
public:
    DataSeriesRepo_Empty();
};

class DataSeriesRepo_DataSeriesNotExist: public std::invalid_argument
{
public:
    DataSeriesRepo_DataSeriesNotExist(const std::string setId);
};

class DataSeriesRepo_DSalreadyExists: public std::invalid_argument
{
public:
    DataSeriesRepo_DSalreadyExists(const std::string setId);
};
} // namespace Antares::Solver::Modeler::DataSeries
