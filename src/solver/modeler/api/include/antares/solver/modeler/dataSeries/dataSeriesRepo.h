
#pragma once

#include <string>
#include <map>
#include "dataSeries.h"

namespace Antares::Solver::Modeler::DataSeries
{

class DataSeriesRepository
{
public:
    void addDataSeries(std::string name, IDataSeries* dataSeries);
    IDataSeries& getDataSeries(std::string name);
private:
    std::map<std::string, IDataSeries*> dataSeries_;
};

}