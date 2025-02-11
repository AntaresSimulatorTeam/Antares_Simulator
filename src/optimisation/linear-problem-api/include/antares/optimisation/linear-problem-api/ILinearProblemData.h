
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimisation::LinearProblemApi
{

struct DataSeriesKeys
{
    std::vector<unsigned int> timeSteps;
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
