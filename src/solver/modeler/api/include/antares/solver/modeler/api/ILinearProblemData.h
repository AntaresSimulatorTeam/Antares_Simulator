
#pragma once

#include <string>
#include <vector>

namespace Antares::Solver::Modeler::Api
{

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

} // namespace Antares::Solver::Modeler::Api
