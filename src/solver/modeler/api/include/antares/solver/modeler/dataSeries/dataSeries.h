
#pragma once

namespace Antares::Solver::Modeler::DataSeries
{

class IDataSeries
{
public:
    virtual double getData(unsigned int set, unsigned int hour) = 0;
};

}
