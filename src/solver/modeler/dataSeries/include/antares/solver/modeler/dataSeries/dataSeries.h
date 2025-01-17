
#pragma once

namespace Antares::Solver::Modeler::DataSeries
{

class IDataSeries
{
public:
    virtual double getData(unsigned int scenario, unsigned int hour) = 0;
};

}
