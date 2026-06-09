// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/inputs/yml-system/system.h"

#include "yaml-cpp/yaml.h"

namespace YAML
{

/**
 * @brief shortend to default construct a value when node is null
 * @tparam T Type to convert the node to
 * @param n node
 * @return Object of type T
 * It's just to simplify repertitve and verbose lines
 * as_fallback_default<std::vector<Antares::IO::Inputs::YmlSystem::Parameter>>(
node["parameters"]) is equivalent to
 node["parameters"].as<std::vector<Antares::IO::Inputs::YmlSystem::Parameter>>(std::vector<Antares::IO::Inputs::YmlSystem::Parameter>())
 */
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

template<>
class convert<Antares::IO::Inputs::YmlSystem::Parameter>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::Parameter& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlSystem::Component>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::Component& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlSystem::Connection>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::Connection& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlSystem::AreaConnection>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::AreaConnection& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlSystem::ThermalComponent>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::ThermalComponent& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection>
{
public:
    static bool decode(const Node& node,
                       Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlSystem::System>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::System& rhs);
};

} // namespace YAML
