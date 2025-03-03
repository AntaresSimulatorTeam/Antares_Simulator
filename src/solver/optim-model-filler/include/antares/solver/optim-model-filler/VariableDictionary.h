/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include <unordered_map>
#include <vector>

#include <antares/solver/optim-model-filler/FullKey.h>

namespace Antares::Optimisation::LinearProblemApi
{
class IMipVariable;
}

namespace Antares::Optimization
{

struct IntegerInterval
{
    unsigned int initialTime = 0;
    unsigned int finalTime = 0;

    class Iterator
    {
    public:
        explicit Iterator(unsigned int current);
        unsigned int operator*() const;
        Iterator& operator++();
        bool operator!=(const Iterator& other) const;

    private:
        unsigned int current_;
    };

    Iterator begin() const
    {
        return Iterator(initialTime);
    }

    Iterator end() const
    {
        return Iterator(finalTime + 1);
    } // Make it inclusive

    std::size_t size() const
    {
        return finalTime - initialTime + 1;
    }
};

class Dimensions
{
public:
    Dimensions() = default;
    Dimensions(std::optional<IntegerInterval> scenarioInterval,
               std::optional<IntegerInterval> timeInterval);
    bool isTimeDependent() const;
    bool isScenarioDependent() const;
    IntegerInterval getTimesteps() const;
    IntegerInterval getScenarioIndices() const;
    unsigned int getNumberOfTimesteps() const;

private:
    std::optional<IntegerInterval> scenarioInterval;
    std::optional<IntegerInterval> timeInterval;
};

struct TimeAndScenario
{
    unsigned int scenario;
    unsigned int timestep;
};

class VariableDictionary
{
    using Value = Antares::Optimisation::LinearProblemApi::IMipVariable*;

    class VectorWithOffset
    {
    public:
        VectorWithOffset() = default;
        void resize(size_t initial_size, unsigned offset);
        Value& operator[](unsigned int index);
        const Value& operator[](unsigned int index) const;
        const Value& at(unsigned int index) const;
        Value& at(unsigned int index);

    private:
        std::vector<Value> values_ = {};
        unsigned int offset_ = 0;
    };

    using TwoIndexVector = std::vector<VectorWithOffset>;
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVector, PartialKeyHash>;

    HashMapVector hmv;
    const TwoIndexVector& operator[](const PartialKey& k) const;

public:
    void addVariable(const Dimensions& dimensions,
                     const PartialKey& key,
                     std::function<Value(const TimeAndScenario&, const std::string&)>&& func);

    Value operator[](const FullKey& k) const;
    Value& operator[](const FullKey& k);

    Value operator()(const std::string& component, const std::string& variable) const;
    Value& operator()(const std::string& component, const std::string& variable);

    Value operator()(const std::string& component,
                     const std::string& variable,
                     unsigned int scenario,
                     unsigned int timestep) const;

    Value& operator()(const std::string& component,
                      const std::string& variable,
                      unsigned int scenario,
                      unsigned int timestep);
    Value operator()(const FullKey& fullKey) const;

    Value& operator()(const FullKey& fullKey);
};
} // namespace Antares::Optimization
