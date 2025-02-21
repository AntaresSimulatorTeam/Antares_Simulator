#pragma once

#include <compare>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

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

template<class Value>
class VariableDict
{
private:
    using TwoIndexVector = std::vector<std::vector<Value>>;
    using HashMapVector = std::unordered_map<PartialKey, TwoIndexVector, hash>;

    HashMapVector hmv;

public:
    inline void addVariable(const FullKey& k,
                            std::function<Value(int, int, const std::string&)>&& func)
    {
        auto& m = hmv[k.getPartialKey()];
        m.resize(k.getTimestep());
        for (int timestep = 0; timestep < k.getTimestep(); ++timestep)
        {
            m[timestep].resize(k.getScenario());
            for (int scenario = 0; scenario < k.getScenario(); ++scenario)
            {
                // TODO allow multiple scenarios
                const std::string name = buildVariableName(k, timestep, scenario);
                m[timestep][scenario] = func(timestep, scenario, name);
            }
        }
    }

    inline Value operator[](const FullKey& k) const
    {
        return hmv.at(k.getPartialKey())[k.getTimestep()][k.getScenario()];
    }

    inline Value& operator[](const FullKey& k)
    {
        return hmv[k.getPartialKey()][k.getTimestep()][k.getScenario()];
    }

    inline const TwoIndexVector& operator[](const PartialKey& k) const
    {
        return hmv.at(k);
    }

    inline Value operator()(const std::string& component,
                            const std::string& variable,
                            int timestep,
                            int scenario) const
    {
        return hmv.at(PartialKey(component, variable))[timestep][scenario];
    }

    inline Value& operator()(const std::string& component,
                             const std::string& variable,
                             int timestep,
                             int scenario)
    {
        return hmv[PartialKey(component, variable)][timestep][scenario];
    }
};
} // namespace Antares::Optimization
