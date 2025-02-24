#include <boost/container_hash/hash.hpp>

#include <antares/solver/optim-model-filler/variableDict.h>

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
    std::string ret = key.getComponent() + "." + key.getVariable();
    if (key.getTimestep() > 1)
    {
        ret += "_" + std::to_string(timestep);
    }
    if (key.getScenario() > 1)
    {
        ret += "_" + std::to_string(scenario);
    }
    return ret;
}
} // namespace Antares::Optimization
