
#pragma once

namespace Antares::Solver::Modeler::DataSeries
{

class IDataSeries
{
public:
    virtual double getData(unsigned int rank, unsigned int hour) = 0;
};

} // namespace Antares::Solver::Modeler::DataSeries
