/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <fmt/format.h>

#include <antares/solver/optim-model-filler/VariableDictionary.h>

namespace Antares::Optimization
{

std::string VariableDictionary::buildVariableName(const PartialKey& key,
                                                  std::optional<MCYearAndTime::MCYear> mcyear,
                                                  std::optional<unsigned int> timestep)
{
    std::string ret = fmt::format("{}.{}", key.getComponent(), key.getVariable());
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

void VariableDictionary::VectorWithOffset::resize(size_t initial_size, unsigned int offset)
{
    offset_ = offset;
    values_.assign(initial_size, nullptr); // initialize all slots to nullptr
}

VariableDictionary::Value& VariableDictionary::VectorWithOffset::operator[](unsigned int index)
{
    return values_[index - offset_];
}

const VariableDictionary::Value& VariableDictionary::VectorWithOffset::operator[](
  unsigned int index) const
{
    return values_[index - offset_];
}

const VariableDictionary::Value& VariableDictionary::VectorWithOffset::at(unsigned int index) const
{
    return values_.at(index - offset_);
}

VariableDictionary::Value& VariableDictionary::VectorWithOffset::at(unsigned int index)
{
    return values_.at(index - offset_);
}

namespace
{
template<typename T>
std::optional<T> buildOptional(bool condition, T value)
{
    if (condition)
    {
        return value;
    }
    else
    {
        return {};
    }
}
} // namespace

void VariableDictionary::addVariable(
  const Dimensions& dimensions,
  const PartialKey& key,
  std::function<Value(const MCYearAndTime&, const std::string&)>&& func)
{
    auto& m = storageOfAddedMipVariables_[key];
    const auto&& scenariosIndices = dimensions.getScenarioIndices();
    const auto time_interval = dimensions.getTimesteps();
    const auto offset = *time_interval.begin();
    for (const auto&& scenario: scenariosIndices)
    {
        auto scenarioNumber = static_cast<MCYearAndTime::MCYear>(scenario);
        m[scenarioNumber].resize(time_interval.size(), offset);
        for (const auto timestep: time_interval)
        {
            auto year = buildOptional<MCYearAndTime::MCYear>(dimensions.isScenarioDependent(),
                                                             scenarioNumber);
            const auto ts = buildOptional(dimensions.isTimeDependent(), timestep);
            const std::string name = buildVariableName(key, year, ts);
            m[scenarioNumber][timestep] = func({.mcYear = scenarioNumber, .timestep = timestep},
                                               name);
        }
    }
}

VariableDictionary::Value VariableDictionary::operator[](const FullKey& k) const
{
    return storageOfAddedMipVariables_.at(k.getPartialKey())
      .at(k.getScenario().value_or(MCYearAndTime::MCYear{0}))
      .at(k.getTimestep().value_or(0));
}

VariableDictionary::Value& VariableDictionary::operator[](const FullKey& k)
{
    return storageOfAddedMipVariables_[k.getPartialKey()]
      .at(k.getScenario().value_or(MCYearAndTime::MCYear{0}))
      .at(k.getTimestep().value_or(0));
}

const VariableDictionary::TwoIndexVectorByYear& VariableDictionary::operator[](
  const PartialKey& k) const
{
    return storageOfAddedMipVariables_.at(k);
}

VariableDictionary::Value VariableDictionary::operator()(const std::string& component,
                                                         const std::string& variable) const
{
    return storageOfAddedMipVariables_.at(PartialKey(component, variable))
      .at(MCYearAndTime::MCYear{0})
      .at(0);
}

VariableDictionary::Value& VariableDictionary::operator()(const std::string& component,
                                                          const std::string& variable)
{
    return storageOfAddedMipVariables_.at(PartialKey(component, variable))
      .at(MCYearAndTime::MCYear{0})
      .at(0);
}

VariableDictionary::Value VariableDictionary::operator()(const std::string& component,
                                                         const std::string& variable,
                                                         const MCYearAndTime::MCYear& scenario,
                                                         unsigned int timestep) const
{
    return storageOfAddedMipVariables_.at(PartialKey(component, variable))
      .at(scenario)
      .at(timestep);
}

VariableDictionary::Value& VariableDictionary::operator()(const std::string& component,
                                                          const std::string& variable,
                                                          const MCYearAndTime::MCYear& scenario,
                                                          unsigned int timestep)
{
    auto&& var = storageOfAddedMipVariables_[PartialKey(component, variable)];
    return var.at(scenario).at(timestep);
}

VariableDictionary::Value VariableDictionary::operator()(const FullKey& fullKey) const
{
    return this->operator()(fullKey.getComponent(),
                            fullKey.getVariable(),
                            fullKey.getScenario().value_or(MCYearAndTime::MCYear{0}),
                            fullKey.getTimestep().value_or(0));
}

VariableDictionary::Value& VariableDictionary::operator()(const FullKey& fullKey)
{
    return this->operator()(fullKey.getComponent(),
                            fullKey.getVariable(),
                            fullKey.getScenario().value_or(MCYearAndTime::MCYear{0}),
                            fullKey.getTimestep().value_or(0));
}
} // namespace Antares::Optimization
