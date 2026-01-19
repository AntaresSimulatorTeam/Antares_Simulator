// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <functional>
#include <optional>
#include <string>

#include "MCYearAndTime.h"

namespace Antares::Optimisation::LinearProblemApi
{
class IMipVariable;
}

namespace Antares::Optimisation
{

struct IntegerInterval
{
    unsigned int initialTime = 0;
    unsigned int finalTime = 0;

    class Iterator final
    {
    public:
        explicit Iterator(unsigned int current);
        unsigned int operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;

    private:
        unsigned int current_;
    };

    [[nodiscard]] Iterator begin() const
    {
        return Iterator(initialTime);
    }

    [[nodiscard]] Iterator end() const
    {
        return Iterator(finalTime + 1);
    } // Make it inclusive

    [[nodiscard]] std::size_t size() const
    {
        return finalTime - initialTime + 1;
    }
};

class Dimensions final
{
public:
    Dimensions() = default;
    Dimensions(std::optional<IntegerInterval> mcyearInterval,
               std::optional<IntegerInterval> timeInterval);
    [[nodiscard]] bool isTimeDependent() const;
    [[nodiscard]] bool isScenarioDependent() const;
    [[nodiscard]] IntegerInterval getTimesteps() const;
    [[nodiscard]] IntegerInterval getScenarioIndices() const;
    [[nodiscard]] unsigned int getNumberOfTimesteps() const;

private:
    std::optional<IntegerInterval> mcyearInterval;
    std::optional<IntegerInterval> timeInterval;
};

// TODO Move me
std::string buildVariableName(const std::string& compoId,
                              const std::string& variableId,
                              std::optional<Optimization::MCYearAndTime::MCYear> mcyear,
                              std::optional<unsigned int> timestep);

} // namespace Antares::Optimisation
