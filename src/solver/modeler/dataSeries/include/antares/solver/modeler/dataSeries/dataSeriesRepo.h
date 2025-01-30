
#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "dataSeries.h"

namespace Antares::Solver::Modeler::DataSeries
{

class DataSeriesRepository
{
public:
    void addDataSeries(std::unique_ptr<IDataSeries> dataSeries);
    IDataSeries& getDataSeries(const std::string& setId);

private:
    std::map<std::string, std::unique_ptr<IDataSeries>> dataSeries_;

public:
    // Exception classes
    class Empty: public std::invalid_argument
    {
    public:
        Empty();
    };

    class DataSeriesNotExist: public std::invalid_argument
    {
    public:
        explicit DataSeriesNotExist(const std::string&);
    };

    class DataSeriesAlreadyExists: public std::invalid_argument
    {
    public:
        explicit DataSeriesAlreadyExists(const std::string&);
    };
};

} // namespace Antares::Solver::Modeler::DataSeries
