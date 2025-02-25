#pragma once

#include <compare>
#include <functional>
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
    const std::optional<int> scenario;
    const std::optional<int> timestep;

    FullKey(const std::string& component, const std::string& variable);
    FullKey(const std::string& component, const std::string& variable, int scenario, int timestep);

    const PartialKey& getPartialKey() const;
    const std::string& getComponent() const;
    const std::string& getVariable() const;

    std::optional<int> getScenario() const;
    std::optional<int> getTimestep() const;

    auto operator<=>(const FullKey&) const = default; // Automatically generates <, >, ==, etc.
};

class hash
{
public:
    std::size_t operator()(const PartialKey& p) const;
};

class Dimensions
{
public:
    struct TimeInterval
    {
        int initialTime;
        int finalTime;
    };

    Dimensions(std::optional<int> nbScenarios, std::optional<TimeInterval> timeInterval);
    bool isTimeDependent() const;
    bool isScenarioDependent() const;
    std::vector<int> getTimesteps() const;
    std::vector<int> getScenarioIndices() const;
    int getNumberOfTimesteps() const;

private:
    std::optional<int> nbScenarios;
    std::optional<TimeInterval> timeInterval;
};

class VariableDictionary
{
public:

private:
    using Value = Antares::Optimisation::LinearProblemApi::IMipVariable*;
    using TwoIndexVector = std::vector<std::vector<Value>>;
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVector, hash>;

    HashMapVector hmv;
    const TwoIndexVector& operator[](const PartialKey& k) const;

public:
    void addVariable(const Dimensions& dimensions,
                     const PartialKey& key,
                     std::function<Value(int, int, const std::string&)>&& func);

    Value operator[](const FullKey& k) const;
    Value& operator[](const FullKey& k);

    Value operator()(const std::string& component, const std::string& variable) const;
    Value& operator()(const std::string& component, const std::string& variable);

    Value operator()(const std::string& component,
                     const std::string& variable,
                     int scenario,
                     int timestep) const;

    Value& operator()(const std::string& component,
                      const std::string& variable,
                      int scenario,
                      int timestep);
};
} // namespace Antares::Optimization
