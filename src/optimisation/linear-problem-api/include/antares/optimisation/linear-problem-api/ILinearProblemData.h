
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

    [[nodiscard]] unsigned getFirstTimeStep() const
    {
        return firstTimeStep;
    }

    [[nodiscard]] unsigned getLastTimeStep() const
    {
        return lastTimeStep;
    }

    [[nodiscard]] unsigned int getNumberOfTimestep() const
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
    virtual ~ILinearProblemData() = default;

    virtual double getData(const std::string& dataSetId,
                           const std::string& scenarioGroup,
                           unsigned scenario,
                           unsigned hour)
      = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
