
#pragma once

#include <string>

namespace Antares::Optimisation::LinearProblemDataImpl
{

class IDataSeries
{
public:
    IDataSeries(std::string name):
        name_(std::move(name))
    {
    }

    virtual double getData(unsigned int rank, unsigned int hour) = 0;

    std::string name() const
    {
        return name_;
    }

private:
    std::string name_;
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
