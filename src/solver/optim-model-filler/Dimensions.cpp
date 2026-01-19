// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>

#include <antares/solver/optim-model-filler/Dimensions.h>

namespace Antares::Optimisation
{

IntegerInterval::Iterator::Iterator(unsigned int current):
    current_(current)
{
}

unsigned int IntegerInterval::Iterator::operator*() const
{
    return current_;
}

IntegerInterval::Iterator& IntegerInterval::Iterator::operator++()
{
    // Prefix increment
    ++current_;
    return *this;
}

bool IntegerInterval::Iterator::operator!=(const Iterator& other) const
{
    return current_ != other.current_;
}

Dimensions::Dimensions(std::optional<IntegerInterval> mcYearInterval,
                       std::optional<IntegerInterval> timeInterval):
    mcyearInterval(mcYearInterval),
    timeInterval(timeInterval)
{
}

bool Dimensions::isTimeDependent() const
{
    return timeInterval.has_value();
}

bool Dimensions::isScenarioDependent() const
{
    return mcyearInterval.has_value();
}

IntegerInterval Dimensions::getTimesteps() const
{
    return timeInterval.value_or(IntegerInterval{});
}

IntegerInterval Dimensions::getScenarioIndices() const
{
    return mcyearInterval.value_or(IntegerInterval{.initialTime = 0, .finalTime = 0});
}

unsigned int Dimensions::getNumberOfTimesteps() const
{
    return timeInterval ? timeInterval->finalTime - timeInterval->initialTime + 1 : 1;
}

std::string buildVariableName(const std::string& compoId,
                              const std::string& variableId,
                              std::optional<Optimization::MCYearAndTime::MCYear> mcyear,
                              std::optional<unsigned int> timestep)
{
    std::string ret = fmt::format("{}.{}", compoId, variableId);
    if (mcyear.has_value())
    {
        ret += "_s" + std::to_string(format_as(mcyear.value()));
    }
    if (timestep.has_value())
    {
        ret += "_t" + std::to_string(*timestep);
    }
    return ret;
}
} // namespace Antares::Optimisation
