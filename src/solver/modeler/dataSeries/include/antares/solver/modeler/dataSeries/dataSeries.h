
#pragma once

#include <string>

namespace Antares::Solver::Modeler::DataSeries
{

class IDataSeries
{
public:
    IDataSeries(std::string name):
        name_(name)
    {
    }

    virtual double getData(unsigned int rank, unsigned int hour) = 0;

    std::string name()
    {
        return name_;
    }

private:
    std::string name_;
};

} // namespace Antares::Solver::Modeler::DataSeries
