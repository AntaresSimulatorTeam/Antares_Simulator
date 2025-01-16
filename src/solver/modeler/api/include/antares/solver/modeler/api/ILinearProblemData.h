
#pragma once

#include <string>

namespace Antares::Solver::Modeler::Api
{

class ILinearProblemData
{
    virtual double getData(std::string idTimeSeriesSet,
                           std::string scenarioGroup,
                           unsigned int scenario,
                           unsigned int hour)
      = 0;
};

}
