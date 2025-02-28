#pragma once

#include <compare>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Antares::Optimisation::LinearProblemApi
{
class IMipVariable;
}

namespace Antares::Optimization
{
struct PartialKey
{
    const std::string component_id;
    const std::string variable_id;

    PartialKey(const std::string& component_id, const std::string& variable_id);

    const std::string& getComponent() const;
    const std::string& getVariable() const;

    auto operator<=>(const PartialKey&) const = default; // Automatically generates <, >, ==, etc.
};

struct FullKey
{
    const PartialKey pk;
    const std::optional<unsigned int> scenario;
    const std::optional<unsigned int> timestep;

    FullKey(const std::string& component, const std::string& variable);
    FullKey(const std::string& component,
            const std::string& variable,
            unsigned int scenario,
            unsigned int timestep);

    const PartialKey& getPartialKey() const;
    const std::string& getComponent() const;
    const std::string& getVariable() const;

    std::optional<unsigned int> getScenario() const;
    std::optional<unsigned int> getTimestep() const;

    auto operator<=>(const FullKey&) const = default; // Automatically generates <, >, ==, etc.
};

class hash
{
public:
    std::size_t operator()(const PartialKey& p) const;
};

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
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVector, hash>;

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
};
} // namespace Antares::Optimization
