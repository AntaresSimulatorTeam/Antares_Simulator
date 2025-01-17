
#pragma once

#include <string>

namespace Antares::Solver::Modeler::DataSeries
{

class IDataSeries
{
public:
    std::string name()
    {
        return name_;
    }

    virtual double getData(unsigned int rank, unsigned int hour) = 0;

private:
    std::string name_;
};

} // namespace Antares::Solver::Modeler::DataSeries
