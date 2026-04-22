// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/decoders.h"

#include <antares/io/inputs/InputError.h>

namespace YAML
{

namespace
{
/// Throws InputError if the node is present but is not a YAML map.
/// Returns false silently when the node is absent or null (permitting as_fallback_default).
bool requireMap(const Node& node, const char* typeName)
{
    if (node.IsMap())
    {
        return true;
    }
    if (node.IsDefined() && !node.IsNull())
    {
        throw Antares::IO::Inputs::InputError(std::string("Expected a YAML mapping for '")
                                              + typeName + "'");
    }
    return false;
}

/// Throws InputError if the map does not have the expected number of fields.
void requireSize(const Node& node, std::size_t expected, const char* typeName)
{
    if (node.size() != expected)
    {
        throw Antares::IO::Inputs::InputError(std::string("Expected ") + std::to_string(expected)
                                              + " field(s) for '" + typeName + "', got "
                                              + std::to_string(node.size()));
    }
}
} // namespace

bool convert<Antares::IO::Inputs::YmlSystem::Parameter>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::Parameter& rhs)
{
    if (!requireMap(node, "parameter"))
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
    if (!requireMap(node, "component"))
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
    if (!requireMap(node, "connection"))
    {
        return false;
    }
    requireSize(node, 4, "connection (component1, port1, component2, port2)");
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
    if (!requireMap(node, "area-connection"))
    {
        return false;
    }
    requireSize(node, 3, "area-connection (component, port, area)");
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    rhs.areaId = node["area"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::ThermalComponent>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::ThermalComponent& rhs)
{
    if (!requireMap(node, "thermal-component"))
    {
        return false;
    }
    requireSize(node, 2, "thermal-component (area, cluster-id)");
    rhs.areaId = node["area"].as<std::string>();
    rhs.clusterId = node["cluster-id"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlSystem::ThermalCapacityConnection& rhs)
{
    if (!requireMap(node, "thermal-capacity-connection"))
    {
        return false;
    }
    requireSize(node, 3, "thermal-capacity-connection (component, port, thermal-component)");
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
    if (!node.IsMap())
    {
        throw Antares::IO::Inputs::InputError("Expected a YAML mapping for 'system'");
    }
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
