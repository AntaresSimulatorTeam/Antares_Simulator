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

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <antares/solver/optim-model-filler/FullKey.h>

#include "MCYearAndTime.h"

namespace Antares::Optimisation::LinearProblemApi
{
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
        explicit Iterator(unsigned int current);
        unsigned int operator*() const;               ///< Current value
        Iterator& operator++();                       ///< Prefix increment
        bool operator!=(const Iterator& other) const; ///< Inequality

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
               std::optional<IntegerInterval> timeInterval);
    [[nodiscard]] bool isTimeDependent() const;         ///< True if time dimension present
    [[nodiscard]] bool isScenarioDependent() const;     ///< True if scenario dimension present
    [[nodiscard]] IntegerInterval getTimesteps() const; ///< Returns time interval or [0,0]
    [[nodiscard]] IntegerInterval getScenarioIndices()
      const;                                                 ///< Returns scenario interval or [0,0]
    [[nodiscard]] unsigned int getNumberOfTimesteps() const; ///< Returns count (1 if scalar)

private:
    std::optional<IntegerInterval> mcyearInterval;
    std::optional<IntegerInterval> timeInterval;
};

/**
 * @brief Storage for created MIP variables indexed by (component, variable, scenario?, timestep?).
 * Uses nested hash maps + a vector with offset for contiguous timestep indices.
 * Provides strict (throwing) accessors and non-throwing tryGet helpers.
 */
class VariableDictionary
{
    using Value = Antares::Optimisation::LinearProblemApi::IMipVariable*; ///< Pointer alias

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

public:
    /**
     * @brief Register a new multi-dimensional variable family.
     * For each (scenario, timestep) in dimensions, the callback is invoked to create the variable.
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
} // namespace Antares::Optimization
