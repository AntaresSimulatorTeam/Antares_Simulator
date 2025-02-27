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
                 unsigned int scenario,
                 unsigned int timestep):
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

std::optional<unsigned int> FullKey::getScenario() const
{
    return scenario;
}

std::optional<unsigned int> FullKey::getTimestep() const
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
                              std::optional<unsigned int> scenario,
                              std::optional<unsigned int> timestep)
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

IntegerInterval::Iterator::Iterator(unsigned int current):
    current_(current)
{
}

unsigned int IntegerInterval::Iterator::operator*() const
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

unsigned int Dimensions::getNumberOfTimesteps() const
{
    return timeInterval ? timeInterval->finalTime - timeInterval->initialTime + 1 : 1;
}

void VariableDictionary::VectorWithOffset::resize(size_t initial_size, unsigned int offset)
{
    offset_ = offset;
    values_.resize(initial_size);
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
std::optional<unsigned int> buildOptional(bool condition, unsigned int value)
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
  std::function<Value(const TimeAndScenario&, const std::string&)>&& func)
{
    auto& m = hmv[key];
    const auto scenarios = dimensions.getScenarioIndices();
    const auto time_interval = dimensions.getTimesteps();
    const auto offset = *time_interval.begin();
    m.resize(scenarios.size());
    for (const auto scenario: scenarios)
    {
        m[scenario].resize(time_interval.size(), offset);
        for (const auto timestep: time_interval)
        {
            const auto sc = buildOptional(dimensions.isScenarioDependent(), scenario);
            const auto ts = buildOptional(dimensions.isTimeDependent(), timestep);
            const std::string name = buildVariableName(key, sc, ts);
            m[scenario][timestep] = func({.scenario = scenario, .timestep = timestep}, name);
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
                                                         unsigned int scenario,
                                                         unsigned int timestep) const
{
    return hmv.at(PartialKey(component, variable)).at(scenario).at(timestep);
}

VariableDictionary::Value& VariableDictionary::operator()(const std::string& component,
                                                          const std::string& variable,
                                                          unsigned int scenario,
                                                          unsigned int timestep)
{
    return hmv[PartialKey(component, variable)].at(scenario).at(timestep);
}

} // namespace Antares::Optimization
