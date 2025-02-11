
#pragma once

#include <string>

namespace Antares::Optimisation::LinearProblemApi
{

class ILinearProblemData
{
    virtual double getData(const std::string& dataSetId,
                           const std::string& scenarioGroup,
                           const unsigned scenario,
                           const unsigned hour)
      = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
