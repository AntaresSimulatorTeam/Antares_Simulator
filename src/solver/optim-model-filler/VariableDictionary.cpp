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
                 int timestep,
                 int scenario):
    pk(component, variable),
    timestep(timestep),
    scenario(scenario)
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

int FullKey::getScenario() const
{
    return scenario;
}

int FullKey::getTimestep() const
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

std::string buildVariableName(const FullKey& key, int timestep, int scenario)
{
    return key.getComponent() + "." + key.getVariable() + "_" + std::to_string(timestep) + "_"
           + std::to_string(scenario);
}

void VariableDictionary::addVariable(const FullKey& k,
                                     std::function<Value(int, int, const std::string&)>&& func)
{
    auto& m = hmv[k.getPartialKey()];
    m.resize(k.getTimestep());
    for (int timestep = 0; timestep < k.getTimestep(); ++timestep)
    {
        m[timestep].resize(k.getScenario());
        for (int scenario = 0; scenario < k.getScenario(); ++scenario)
        {
            const std::string name = buildVariableName(k, timestep, scenario);
            m[timestep][scenario] = func(timestep, scenario, name);
        }
    }
}

VariableDictionary::Value VariableDictionary::operator[](const FullKey& k) const
{
    return hmv.at(k.getPartialKey())[k.getTimestep()][k.getScenario()];
}

VariableDictionary::Value& VariableDictionary::operator[](const FullKey& k)
{
    return hmv[k.getPartialKey()][k.getTimestep()][k.getScenario()];
}

const VariableDictionary::TwoIndexVector& VariableDictionary::operator[](const PartialKey& k) const
{
    return hmv.at(k);
}

VariableDictionary::Value VariableDictionary::operator()(const std::string& component,
                                                         const std::string& variable,
                                                         int timestep,
                                                         int scenario) const
{
    return hmv.at(PartialKey(component, variable))[timestep][scenario];
}

VariableDictionary::Value& VariableDictionary::operator()(const std::string& component,
                                                          const std::string& variable,
                                                          int timestep,
                                                          int scenario)
{
    return hmv[PartialKey(component, variable)][timestep][scenario];
}

} // namespace Antares::Optimization
