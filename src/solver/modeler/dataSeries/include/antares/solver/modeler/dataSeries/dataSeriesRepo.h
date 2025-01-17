
#pragma once

#include <map>
#include <string>

#include "dataSeries.h"

namespace Antares::Solver::Modeler::DataSeries
{

class DataSeriesRepository
{
public:
    void addDataSeries(std::string setId, IDataSeries* dataSeries);
    IDataSeries& getDataSeries(std::string setId);

private:
    std::map<std::string, IDataSeries*> dataSeries_;
};

} // namespace Antares::Solver::Modeler::DataSeries
