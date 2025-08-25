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

#pragma once

#include <fmt/format.h>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <antares/solver/optim-model-filler/FullKey.h>
#include "antares/optimisation/linear-problem-api/mipVariable.h"

#include "MCYearAndTime.h"

namespace Antares::Optimisation::LinearProblemApi
{
template<SolverVariable InnerSolverVariable>
class IMipVariable;
}

namespace Antares::Optimization
{
/**
 * @brief Closed integer interval [initialTime, finalTime] with inclusive end iterator semantics.
 */
struct IntegerInterval
{
    unsigned int initialTime = 0; ///< First value in interval
    unsigned int finalTime = 0;   ///< Last value (inclusive)

    class Iterator
    {
    public:
        explicit Iterator(unsigned int current):
            current_(current)
        {
        }

        unsigned int operator*() const
        {
            return current_;
        }

        Iterator& operator++()
        {
            ++current_;
            return *this;
        }

        bool operator!=(const Iterator& other) const
        {
            return current_ != other.current_;
        }

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
    } // inclusive range

    [[nodiscard]] std::size_t size() const
    {
        return finalTime - initialTime + 1;
    }
};

/**
 * @brief Describes dimensional dependence (scenario/time) of a variable family.
 * If an optional interval is missing, that dimension is considered scalar (size 1).
 */
class Dimensions
{
public:
    Dimensions() = default;

    Dimensions(std::optional<IntegerInterval> mcyearInterval,
               std::optional<IntegerInterval> timeInterval):
        mcyearInterval(mcyearInterval),
        timeInterval(timeInterval)
    {
    }

    [[nodiscard]] bool isTimeDependent() const
    {
        return timeInterval.has_value();
    }

    [[nodiscard]] bool isScenarioDependent() const
    {
        return mcyearInterval.has_value();
    }

    [[nodiscard]] IntegerInterval getTimesteps() const
    {
        return timeInterval.value_or(IntegerInterval{});
    }

    [[nodiscard]] IntegerInterval getScenarioIndices() const
    {
        return mcyearInterval.value_or(IntegerInterval{.initialTime = 0, .finalTime = 0});
    }

    [[nodiscard]] unsigned int getNumberOfTimesteps() const
    {
        return timeInterval ? timeInterval->finalTime - timeInterval->initialTime + 1 : 1;
    }

private:
    std::optional<IntegerInterval> mcyearInterval;
    std::optional<IntegerInterval> timeInterval;
};

/**
 * @brief Storage for created MIP variables indexed by (component, variable, scenario?, timestep?).
 * Uses nested hash maps + a vector with offset for contiguous timestep indices.
 * Provides strict (throwing) accessors and non-throwing tryGet helpers.
 */
template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
class VariableDictionary
{
    using Value = Antares::Optimisation::LinearProblemApi::IMipVariable<
      InnerSolverVariable>*; ///< Pointer alias

    class VectorWithOffset
    {
    public:
        VectorWithOffset() = default;
        /** @brief Resize underlying storage; new slots initialised to nullptr. */
        void resize(size_t initial_size, unsigned offset);
        Value& operator[](unsigned int index); ///< Unchecked access (existing)
        [[nodiscard]] const Value& operator[](unsigned int index) const; ///< Unchecked const
        [[nodiscard]] const Value& at(unsigned int index) const;         ///< Bounds-checked const
        Value& at(unsigned int index);                                   ///< Bounds-checked
        [[nodiscard]] bool contains(unsigned int index) const noexcept;  ///< Fast bounds test

    private:
        std::vector<Value> values_ = {};
        unsigned int offset_ = 0; ///< Logical start index used for mapping
    };

    using TwoIndexVectorByYear = std::unordered_map<MCYearAndTime::MCYear, VectorWithOffset>;
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVectorByYear, PartialKeyHash>;

    HashMapVector storageOfAddedMipVariables_;                         ///< Main storage
    const TwoIndexVectorByYear& operator[](const PartialKey& k) const; ///< Internal helper
    mutable StringToIdMapper mapper_;                                  ///< For naming

public:
    PartialKey buildKey(std::string_view component, std::string_view name) const;

    template<typename... Args>
    FullKey buildFullKey(Args... args) const;

    /**
     * @brief Register a new multi-dimensional variable family.
     * For each (scenario, timestep) in dimensions, the callback is invoked to create the
     * variable.
     * @param dimensions Dimensional metadata.
     * @param key Partial key (component, variable).
     * @param func Factory: (MCYearAndTime, generatedName) -> variable pointer.
     */
    void addVariable(const Dimensions& dimensions,
                     const PartialKey& key,
                     std::function<Value(const MCYearAndTime&, const std::string&)>&& func);

    /** Strict accessors (throw std::out_of_range if missing). */
    Value operator[](const FullKey& k) const;
    Value& operator[](const FullKey& k);

    /** Convenience scalar (scenario/time = 0) accessors (strict). */
    Value operator()(const std::string& component, const std::string& variable) const;
    Value& operator()(const std::string& component, const std::string& variable);

    /** Full dimension strict accessors. */
    Value operator()(const std::string& component,
                     const std::string& variable,
                     const MCYearAndTime::MCYear& scenario,
                     unsigned int timestep) const;
    Value& operator()(const std::string& component,
                      const std::string& variable,
                      const MCYearAndTime::MCYear& scenario,
                      unsigned int timestep);

    /** Overload for FullKey (strict). */
    Value operator()(const FullKey& fullKey) const;
    Value& operator()(const FullKey& fullKey);
};

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
template<typename... Args>
FullKey VariableDictionary<InnerSolverVariable>::buildFullKey(Args... args) const
{
    return FullKey(std::forward<Args>(args)..., mapper_);
}

// Implémentation des méthodes template
std::string buildVariableName(const PartialKey& key,
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

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
void VariableDictionary<InnerSolverVariable>::VectorWithOffset::resize(size_t initial_size,
                                                                       unsigned int offset)
{
    offset_ = offset;
    values_.assign(initial_size, nullptr);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::VectorWithOffset::operator[](unsigned int index)
{
    return values_[index - offset_];
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
const typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::VectorWithOffset::operator[](unsigned int index) const
{
    return values_[index - offset_];
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
const typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::VectorWithOffset::at(unsigned int index) const
{
    return values_.at(index - offset_);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::VectorWithOffset::at(unsigned int index)
{
    return values_.at(index - offset_);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
bool VariableDictionary<InnerSolverVariable>::VectorWithOffset::contains(
  unsigned int index) const noexcept
{
    if (values_.empty() || index < offset_)
    {
        return false;
    }
    auto pos = index - offset_;
    return pos < values_.size();
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
PartialKey VariableDictionary<InnerSolverVariable>::buildKey(std::string_view component,
                                                             std::string_view name) const
{
    return {component, name, mapper_};
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
void VariableDictionary<InnerSolverVariable>::addVariable(
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
            const auto year = buildOptional<MCYearAndTime::MCYear>(dimensions.isScenarioDependent(),
                                                                   scenarioNumber);
            const auto ts = buildOptional(dimensions.isTimeDependent(), timestep);
            const std::string name = buildVariableName(key, year, ts);
            m[scenarioNumber][timestep] = func({.mcYear = scenarioNumber, .timestep = timestep},
                                               name);
        }
    }
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value
VariableDictionary<InnerSolverVariable>::operator[](const FullKey& k) const
{
    return storageOfAddedMipVariables_.at(k.getPartialKey())
      .at(k.getScenario().value_or(MCYearAndTime::MCYear{0}))
      .at(k.getTimestep().value_or(0));
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::operator[](const FullKey& k)
{
    return storageOfAddedMipVariables_[k.getPartialKey()]
      .at(k.getScenario().value_or(MCYearAndTime::MCYear{0}))
      .at(k.getTimestep().value_or(0));
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
const typename VariableDictionary<InnerSolverVariable>::TwoIndexVectorByYear&
VariableDictionary<InnerSolverVariable>::operator[](const PartialKey& k) const
{
    return storageOfAddedMipVariables_.at(k);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value
VariableDictionary<InnerSolverVariable>::operator()(const std::string& component,
                                                    const std::string& variable) const
{
    return storageOfAddedMipVariables_.at(PartialKey(component, variable, mapper_))
      .at(MCYearAndTime::MCYear{0})
      .at(0);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::operator()(const std::string& component,
                                                    const std::string& variable)
{
    return storageOfAddedMipVariables_.at(PartialKey(component, variable, mapper_))
      .at(MCYearAndTime::MCYear{0})
      .at(0);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value
VariableDictionary<InnerSolverVariable>::operator()(const std::string& component,
                                                    const std::string& variable,
                                                    const MCYearAndTime::MCYear& scenario,
                                                    unsigned int timestep) const
{
    return storageOfAddedMipVariables_.at(PartialKey(component, variable, mapper_))
      .at(scenario)
      .at(timestep);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::operator()(const std::string& component,
                                                    const std::string& variable,
                                                    const MCYearAndTime::MCYear& scenario,
                                                    unsigned int timestep)
{
    auto&& var = storageOfAddedMipVariables_[PartialKey(component, variable, mapper_)];
    return var.at(scenario).at(timestep);
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value
VariableDictionary<InnerSolverVariable>::operator()(const FullKey& fullKey) const
{
    return this->operator()(fullKey.getComponent(),
                            fullKey.getVariable(),
                            fullKey.getScenario().value_or(MCYearAndTime::MCYear{0}),
                            fullKey.getTimestep().value_or(0));
}

template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
typename VariableDictionary<InnerSolverVariable>::Value&
VariableDictionary<InnerSolverVariable>::operator()(const FullKey& fullKey)
{
    return this->operator()(fullKey.getComponent(),
                            fullKey.getVariable(),
                            fullKey.getScenario().value_or(MCYearAndTime::MCYear{0}),
                            fullKey.getTimestep().value_or(0));
}
} // namespace Antares::Optimization
