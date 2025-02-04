
#pragma once

#include <string>

namespace Antares::Solver::Modeler::Api
{

class ILinearProblemData
{
    virtual double getData(const std::string& dataSetId,
                           const std::string& scenarioGroup,
                           const unsigned scenario,
                           const unsigned hour)
      = 0;
};

} // namespace Antares::Solver::Modeler::Api
