
#pragma once

#include <string>
#include <vector>

namespace Antares::Optimisation::LinearProblemApi
{
class FillContext
{
public:
    FillContext(unsigned first, unsigned last, unsigned year, std::string group):
        firstTimeStep(first),
        lastTimeStep(last),
    year_{year},
    group_{std::move(group)}
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

    [[nodiscard]] std::string getGroup() const {
        return group_;
    }

    [[nodiscard]] unsigned getYear() const {
        return year_;
    }

private:
    std::vector<unsigned> selectedScenario;
    std::string group_{};
    unsigned year_{0};

    unsigned firstTimeStep = 0;
    unsigned lastTimeStep = 0;
};

class ILinearProblemData
{
public:
    virtual ~ILinearProblemData() = default;

    [[nodiscard]] virtual double getData(const std::string& dataSetId,
                                         unsigned timeSeriesNumber,
                                         unsigned hour) const
      = 0;
};

} // namespace Antares::Optimisation::LinearProblemApi
