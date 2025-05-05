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

#include "antares/io/inputs/yml-system/system.h"

#include "yaml-cpp/yaml.h"

// Implement convert specializations
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
struct convert<Antares::IO::Inputs::YmlSystem::Parameter>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::Parameter& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.time_dependent = node["time-dependent"].as<bool>();
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>();
        rhs.value = node["value"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlSystem::Component>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::Component& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.model = node["model"].as<std::string>();
        rhs.scenarioGroup = node["scenario-group"].as<std::string>();
        rhs.parameters = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlSystem::Parameter>>(node["parameters"]);
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlSystem::Connection>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::Connection& rhs)
    {
        if (!node.IsMap() && node.size() != 4)
        {
            return false;
        }
        rhs.firstEntry.componentId = node["component1"].as<std::string>();
        rhs.firstEntry.portId = node["port1"].as<std::string>();
        rhs.secondEntry.componentId = node["component2"].as<std::string>();
        rhs.secondEntry.portId = node["port2"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlSystem::AreaConnection>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::AreaConnection& rhs)
    {
        if (!node.IsMap() && node.size() != 4)
        {
            return false;
        }
        rhs.componentId = node["component"].as<std::string>();
        rhs.portId = node["port"].as<std::string>();
        rhs.areaId = node["area"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlSystem::System>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlSystem::System& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        rhs.libraries = as_fallback_default<std::vector<std::string>>(node["model-libraries"]);
        rhs.components = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlSystem::Component>>(node["components"]);
        rhs.connections = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlSystem::Connection>>(node["connections"]);
        rhs.areaConnections = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlSystem::AreaConnection>>(node["area-connections"]);
        return true;
    }
};

} // namespace YAML
