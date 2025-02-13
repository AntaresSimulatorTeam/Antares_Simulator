
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimisation::LinearProblemApi
{
struct FillContext
{
    FillContext(unsigned first, unsigned last):
        firstTimeStep(first),
        lastTimeStep(last)
    {
    }

    unsigned getFirstTimeStep() const
    {
        return firstTimeStep;
    }

    unsigned getLastTimeStep() const
    {
        return lastTimeStep;
    }

    std::vector<unsigned> scenariosSelected;

    unsigned int getNumberOfTimestep() const
    {
        return lastTimeStep - firstTimeStep + 1;
    }

private:
    unsigned firstTimeStep = 0;
    unsigned lastTimeStep = 0;
};

struct DataSeriesKeys
{
    FillContext fillContext;
    std::string scenarioGroup;
    unsigned scenario;
};

class ILinearProblemData
{
public:
    virtual double getData(const std::string& dataSetId,
                           const std::string& scenarioGroup,
                           const unsigned scenario,
                           const unsigned hour)
      = 0;
    virtual std::vector<double> getData(const std::string& dataSetId,
                                        const std::string& scenarioGroup,
                                        const unsigned scenario)
      = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
