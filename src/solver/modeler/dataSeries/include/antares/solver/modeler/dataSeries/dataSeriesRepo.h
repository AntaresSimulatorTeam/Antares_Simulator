
#pragma once

#include <map>
#include <memory>
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
};

} // namespace Antares::Solver::Modeler::DataSeries
