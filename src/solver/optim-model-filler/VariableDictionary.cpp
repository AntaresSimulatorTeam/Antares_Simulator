#include <boost/container_hash/hash.hpp>

#include <antares/solver/optim-model-filler/VariableDictionary.h>

namespace Antares::Optimization
{

// PartialKey
PartialKey::PartialKey(const std::string& component_id, const std::string& variable_id):
    component_id(component_id),
    variable_id(variable_id)
{
}

const std::string& PartialKey::getComponent() const
{
    return component_id;
}

const std::string& PartialKey::getVariable() const
{
    return variable_id;
}

// FullKey
FullKey::FullKey(const std::string& component,
                 const std::string& variable,
                 int scenario,
                 int timestep):
    pk(component, variable),
    scenario(scenario),
    timestep(timestep)
{
}

FullKey::FullKey(const std::string& component, const std::string& variable):
    pk(component, variable)
{
}

const PartialKey& FullKey::getPartialKey() const
{
    return pk;
}

const std::string& FullKey::getComponent() const
{
    return pk.component_id;
}

const std::string& FullKey::getVariable() const
{
    return pk.variable_id;
}

std::optional<int> FullKey::getScenario() const
{
    return scenario;
}

std::optional<int> FullKey::getTimestep() const
{
    return timestep;
}

// hash

std::size_t hash::operator()(const PartialKey& p) const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, p.component_id);
    boost::hash_combine(seed, p.variable_id);
    return seed;
}

std::string buildVariableName(const PartialKey& key,
                              std::optional<int> scenario,
                              std::optional<int> timestep)
{
    std::string ret = key.getComponent() + "." + key.getVariable();
    if (scenario.has_value())
    {
        ret += "_s" + std::to_string(*scenario);
    }
    if (timestep.has_value())
    {
        ret += "_t" + std::to_string(*timestep);
    }
    return ret;
}

IntegerInterval::Iterator::Iterator(int current):
    current_(current)
{
}

int IntegerInterval::Iterator::operator*() const
{
    return current_;
}

IntegerInterval::Iterator& IntegerInterval::Iterator::operator++()
{ // Prefix increment
    ++current_;
    return *this;
}

bool IntegerInterval::Iterator::operator!=(const Iterator& other) const
{
    return current_ != other.current_;
}

Dimensions::Dimensions(std::optional<IntegerInterval> scenarioInterval,
                       std::optional<IntegerInterval> timeInterval):
    scenarioInterval(scenarioInterval),
    timeInterval(timeInterval)
{
}

bool Dimensions::isTimeDependent() const
{
    return timeInterval.has_value();
}

bool Dimensions::isScenarioDependent() const
{
    return scenarioInterval.has_value();
}

IntegerInterval Dimensions::getTimesteps() const
{
    return timeInterval.value_or(IntegerInterval{});
}

IntegerInterval Dimensions::getScenarioIndices() const
{
    return scenarioInterval.value_or(IntegerInterval{});
}

int Dimensions::getNumberOfTimesteps() const
{
    return timeInterval ? timeInterval->finalTime - timeInterval->initialTime + 1 : 1;
}

namespace
{
std::optional<int> buildOptional(bool condition, int value)
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

void VariableDictionary::addVariable(const Dimensions& dimensions,
                                     const PartialKey& key,
                                     std::function<Value(int, int, const std::string&)>&& func)
{
    auto& m = hmv[key];
    const auto scenarios = dimensions.getScenarioIndices();
    const auto timesteps = dimensions.getTimesteps();
    const int offset = *timesteps.begin();
    m.resize(scenarios.size());
    for (int scenario: scenarios)
    {
        m[scenario].resize(timesteps.size());

        for (int timestep: timesteps)
        {
            const auto sc = buildOptional(dimensions.isScenarioDependent(), scenario);
            const auto ts = buildOptional(dimensions.isTimeDependent(), timestep);
            const std::string name = buildVariableName(key, sc, ts);
            m[scenario][timestep - offset] = func(scenario, timestep, name);
        }
    }
}

VariableDictionary::Value VariableDictionary::operator[](const FullKey& k) const
{
    return hmv.at(k.getPartialKey())
      .at(k.getScenario().value_or(0))
      .at(k.getTimestep().value_or(0));
}

VariableDictionary::Value& VariableDictionary::operator[](const FullKey& k)
{
    return hmv[k.getPartialKey()].at(k.getScenario().value_or(0)).at(k.getTimestep().value_or(0));
}

const VariableDictionary::TwoIndexVector& VariableDictionary::operator[](const PartialKey& k) const
{
    return hmv.at(k);
}

VariableDictionary::Value VariableDictionary::operator()(const std::string& component,
                                                         const std::string& variable) const
{
    return hmv.at(PartialKey(component, variable)).at(0).at(0);
}

VariableDictionary::Value& VariableDictionary::operator()(const std::string& component,
                                                          const std::string& variable)
{
    return hmv.at(PartialKey(component, variable)).at(0).at(0);
}

VariableDictionary::Value VariableDictionary::operator()(const std::string& component,
                                                         const std::string& variable,
                                                         int scenario,
                                                         int timestep) const
{
    return hmv.at(PartialKey(component, variable)).at(scenario).at(timestep);
}

VariableDictionary::Value& VariableDictionary::operator()(const std::string& component,
                                                          const std::string& variable,
                                                          int scenario,
                                                          int timestep)
{
    return hmv[PartialKey(component, variable)].at(scenario).at(timestep);
}

} // namespace Antares::Optimization
