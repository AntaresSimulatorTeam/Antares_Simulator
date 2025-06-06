#pragma once

#include <string>

namespace Antares::Optimisation::LinearProblemApi
{
class IScenario
{
public:
    using Year = unsigned;
    using TimeSeriesNumber = unsigned;

    virtual ~IScenario() = default;

    explicit IScenario(std::string group):
        group_(std::move(group))
    {
    }

    [[nodiscard]] virtual TimeSeriesNumber getData(Year year) const = 0;

    [[nodiscard]] std::string group() const
    {
        return group_;
    }

private:
    std::string group_;
};

    class EmptyScenario : public IScenario {
    public:
        EmptyScenario(): IScenario("empty") {
        }

        [[nodiscard]] TimeSeriesNumber getData(Year) const override {
            return 0; // No data available in an empty scenario
        }
    };
} // namespace Antares::Optimisation::LinearProblemApi
