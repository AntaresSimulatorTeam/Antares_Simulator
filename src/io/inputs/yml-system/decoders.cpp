// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/decoders.h"

namespace YAML
{

bool convert<Antares::IO::Inputs::YmlSystem::Parameter>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::Parameter& rhs)
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

bool convert<Antares::IO::Inputs::YmlSystem::Component>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::Component& rhs)
{
    if (!node.IsMap())
    {
        return false;
    }
    rhs.id = node["id"].as<std::string>();
    rhs.model = node["model"].as<std::string>();
    rhs.scenarioGroup = node["scenario-group"].as<std::string>("");
    rhs.parameters = as_fallback_default<std::vector<Antares::IO::Inputs::YmlSystem::Parameter>>(
      node["parameters"]);
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::Connection>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::Connection& rhs)
{
    if (!node.IsMap() || node.size() != 4)
    {
        return false;
    }
    rhs.firstEntry.componentId = node["component1"].as<std::string>();
    rhs.firstEntry.portId = node["port1"].as<std::string>();

    rhs.secondEntry.componentId = node["component2"].as<std::string>();
    rhs.secondEntry.portId = node["port2"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::AreaConnection>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::AreaConnection& rhs)
{
    if (!node.IsMap() || node.size() != 3)
    {
        return false;
    }
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    rhs.areaId = node["area"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::ThermalComponent>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::ThermalComponent& rhs)
{
    if (!node.IsMap() || node.size() != 2)
    {
        return false;
    }
    rhs.areaId = node["area"].as<std::string>();
    rhs.clusterId = node["cluster-id"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection& rhs)
{
    if (!node.IsMap() || node.size() != 3)
    {
        return false;
    }
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    rhs.thermalComponent = as_fallback_default<Antares::IO::Inputs::YmlSystem::ThermalComponent>(
      node["thermal-component"]);
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::System>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::System& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.libraries = as_fallback_default<std::vector<std::string>>(node["model-libraries"]);
    rhs.components = as_fallback_default<std::vector<Antares::IO::Inputs::YmlSystem::Component>>(
      node["components"]);
    rhs.connections = as_fallback_default<std::vector<Antares::IO::Inputs::YmlSystem::Connection>>(
      node["connections"]);
    rhs.areaConnections = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlSystem::AreaConnection>>(node["area-connections"]);
    rhs.thermalCapacityConnections = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection>>(
      node["thermal-capacity-connections"]);
    return true;
}

} // namespace YAML

