#pragma once

#include <compare>
#include <functional>
#include <string>
#include <unordered_map>
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
    const int timestep;
    const int scenario;

    FullKey(const std::string& component, const std::string& variable, int timestep, int scenario);

    const PartialKey& getPartialKey() const;
    const std::string& getComponent() const;
    const std::string& getVariable() const;

    int getScenario() const;
    int getTimestep() const;

    auto operator<=>(const FullKey&) const = default; // Automatically generates <, >, ==, etc.
};

class hash
{
public:
    std::size_t operator()(const PartialKey& p) const;
};

std::string buildVariableName(const FullKey& key, int timestep, int scenario);

class VariableDictionary
{
private:
    using Value = Antares::Optimisation::LinearProblemApi::IMipVariable*;
    using TwoIndexVector = std::vector<std::vector<Value>>;
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVector, hash>;

    HashMapVector hmv;

public:
    void addVariable(const FullKey& k, std::function<Value(int, int, const std::string&)>&& func);
    Value operator[](const FullKey& k) const;
    Value& operator[](const FullKey& k);

    const TwoIndexVector& operator[](const PartialKey& k) const;

    Value operator()(const std::string& component,
                     const std::string& variable,
                     int timestep,
                     int scenario) const;

    Value& operator()(const std::string& component,
                      const std::string& variable,
                      int timestep,
                      int scenario);
};
} // namespace Antares::Optimization
