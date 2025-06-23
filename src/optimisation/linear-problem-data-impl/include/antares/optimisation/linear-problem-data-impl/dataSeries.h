
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimisation::LinearProblemDataImpl
{

class IDataSeries
{
public:
    virtual ~IDataSeries() = default;

    explicit IDataSeries(std::string name):
        name_(std::move(name))
    {
    }

    virtual double getData(unsigned int rank, unsigned int hour) const = 0;

    std::string name() const
    {
        return name_;
    }

private:
    std::string name_;
};

} // namespace Antares::Optimisation::LinearProblemDataImpl
