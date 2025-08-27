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

#include <cstdint>
#include <fmt/format.h>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <antares/solver/optim-model-filler/FullKey.h>
#include <antares/solver/optim-model-filler/StringToIdMapper.h>
#include "antares/optimisation/linear-problem-api/mipVariable.h"

#include "MCYearAndTime.h"

namespace Antares::Optimisation::LinearProblemApi
{
template<SolverVariable InnerSolverVariable>
class IMipVariable;
}

namespace Antares::Optimization
{
// Type compact pour les index de variables
using VarIndex = uint32_t;

/**
 * @brief Handle compact pour accès rapide aux variables d'une même famille
 * Stocke les informations nécessaires pour calculer l'index d'une variable en O(1)
 */
struct KeyHandle
{
    VarIndex base;   // index de base dans storage_
    uint16_t firstT; // premier pas de temps
    uint16_t countT; // nombre de pas de temps
    uint16_t firstY; // première année/scenario
    uint16_t countY; // nombre d'années/scenarios
};

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

    [[nodiscard]] bool empty() const
    {
        return finalTime < initialTime;
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

// Implémentation des méthodes template
inline std::string buildVariableName(const PartialKey& key,
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

inline std::string buildVariableName(const PartialKey& key, MCYearAndTime mcyearAndTime)
{
    std::string ret = fmt::format("{}.{}", key.getComponent(), key.getVariable());
    ret += "_s" + std::to_string(format_as(mcyearAndTime.mcYear));
    ret += "_t" + std::to_string(mcyearAndTime.timestep);
    return ret;
}

/**
 * @brief Storage for created MIP variables indexed by (component, variable, scenario?, timestep?).
 * Uses nested hash maps + a vector with offset for contiguous timestep indices.
 * Provides strict (throwing) accessors and non-throwing tryGet helpers.
 * Optimized version with O(1) access by index for hot path performance.
 */
template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
class VariableDictionary
{
    using Value = Antares::Optimisation::LinearProblemApi::IMipVariable<
      InnerSolverVariable>*; ///< Pointer alias

    // Legacy storage for backward compatibility
    class VectorWithOffset
    {
    public:
        VectorWithOffset() = default;
        void resize(size_t initial_size, unsigned offset);
        Value& operator[](unsigned int index);
        [[nodiscard]] const Value& operator[](unsigned int index) const;
        [[nodiscard]] const Value& at(unsigned int index) const;
        Value& at(unsigned int index);
        [[nodiscard]] bool contains(unsigned int index) const noexcept;

    private:
        std::vector<Value> values_ = {};
        unsigned int offset_ = 0;
    };

    using TwoIndexVectorByYear = std::unordered_map<MCYearAndTime::MCYear, VectorWithOffset>;
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVectorByYear, PartialKeyHash>;

    // Legacy storage (kept for compatibility)
    HashMapVector storageOfAddedMipVariables_;
    // New optimized storage
    std::vector<Value> storage_; ///< O(1) access by index
    std::unordered_map<PartialKey, KeyHandle, PartialKeyHash>
      keyHandleMap_; ///< PartialKey -> Handle resolution

    mutable StringToIdMapper mapper_; ///< For naming

    const TwoIndexVectorByYear& operator[](const PartialKey& k) const;

public:
    /**
     * @brief Optimized O(1) access by variable index (hot path)
     */
    Value byIndex(VarIndex idx) const
    {
        return storage_[idx];
    }

    /**
     * @brief Get handle for a PartialKey (resolve once per PartialKey)
     */
    const KeyHandle& handle(const PartialKey& key) const
    {
        return keyHandleMap_.at(key);
    }

    /**
     * @brief Calculate variable index from handle and time/scenario (O(1))
     */
    VarIndex indexOf(const KeyHandle& h, const MCYearAndTime& t) const
    {
        const uint32_t yOff = static_cast<uint32_t>(static_cast<uint32_t>(t.mcYear) - h.firstY);
        const uint32_t tOff = static_cast<uint32_t>(t.timestep - h.firstT);
        return static_cast<VarIndex>(h.base + yOff * h.countT + tOff);
    }

    PartialKey buildKey(std::string_view component, std::string_view name) const;

    template<typename... Args>
    FullKey buildFullKey(Args... args) const;

    /**
     * @brief Register a new multi-dimensional variable family with optimized storage
     */
    template<typename Factory>
    void addVariable(const Dimensions& dimensions, const PartialKey& key, Factory&& factory);

    /** Legacy strict accessors (kept for backward compatibility) */
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

// Template implementations
template<Antares::Optimisation::LinearProblemApi::SolverVariable InnerSolverVariable>
template<typename... Args>
FullKey VariableDictionary<InnerSolverVariable>::buildFullKey(Args... args) const
{
    return FullKey(std::forward<Args>(args)..., mapper_);
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
template<typename Factory>
void VariableDictionary<InnerSolverVariable>::addVariable(const Dimensions& dimensions,
                                                          const PartialKey& key,
                                                          Factory&& factory)
{
    // Optimized storage: calculate bounds and store in vector with index
    const auto timeInterval = dimensions.getTimesteps();
    const auto scenarioInterval = dimensions.getScenarioIndices();

    const uint16_t firstT = timeInterval.empty() ? 0 : static_cast<uint16_t>(*timeInterval.begin());
    const uint16_t countT = static_cast<uint16_t>(dimensions.getNumberOfTimesteps());
    const uint16_t firstY = scenarioInterval.empty()
                              ? 0
                              : static_cast<uint16_t>(*scenarioInterval.begin());
    const uint16_t countY = static_cast<uint16_t>(scenarioInterval.size());

    const VarIndex base = static_cast<VarIndex>(storage_.size());
    keyHandleMap_[key] = KeyHandle{base, firstT, countT, firstY, countY};

    // Reserve space for all variables in this family
    const size_t totalVars = static_cast<size_t>(countY ? countY : 1)
                             * static_cast<size_t>(countT ? countT : 1);
    storage_.reserve(storage_.size() + totalVars);

    // Matrix layout: (scenario, timestep)
    for (uint16_t y = 0; y < (countY ? countY : 1); ++y)
    {
        const auto year = static_cast<MCYearAndTime::MCYear>(firstY + y);
        for (uint16_t k = 0; k < (countT ? countT : 1); ++k)
        {
            const uint16_t ts = firstT + k;
            const MCYearAndTime yts{year, ts};
            const std::string name = buildVariableName(key, yts);
            storage_.push_back(factory(yts, name));
        }
    }

    // Also maintain legacy storage for backward compatibility
    auto& m = storageOfAddedMipVariables_[key];
    const auto&& scenariosIndices = dimensions.getScenarioIndices();
    const auto offset = *timeInterval.begin();
    for (const auto&& scenario: scenariosIndices)
    {
        auto scenarioNumber = static_cast<MCYearAndTime::MCYear>(scenario);
        m[scenarioNumber].resize(timeInterval.size(), offset);
        for (const auto timestep: timeInterval)
        {
            const auto year = buildOptional<MCYearAndTime::MCYear>(dimensions.isScenarioDependent(),
                                                                   scenarioNumber);
            const auto ts = buildOptional(dimensions.isTimeDependent(), timestep);
            const std::string name = buildVariableName(key, year, ts);
            m[scenarioNumber][timestep] = factory({.mcYear = scenarioNumber, .timestep = timestep},
                                                  name);
        }
    }
}

// Legacy storage implementation
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
