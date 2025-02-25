
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimisation::LinearProblemApi
{
class FillContext
{
public:
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

    unsigned int getNumberOfTimestep() const
    {
        return lastTimeStep - firstTimeStep + 1;
    }

    [[nodiscard]] std::vector<unsigned> getSelectedScenarios() const
    {
        return selectedScenario;
    }

    void addSelectedScenarios(unsigned scenario)
    {
        selectedScenario.push_back(scenario);
    }

private:
    std::vector<unsigned> selectedScenario;

    unsigned firstTimeStep = 0;
    unsigned lastTimeStep = 0;
};

class ILinearProblemData
{
public:
    virtual double getData(const std::string& dataSetId,
                           const std::string& scenarioGroup,
                           const unsigned scenario,
                           const unsigned hour)
      = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
