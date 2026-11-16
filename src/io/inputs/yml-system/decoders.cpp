// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/decoders.h"

#include "include/antares/io/inputs/yml-system/system.h"

using namespace Antares::IO::Inputs;

namespace YAML
{

// Keep field reads in sync with the validation schema in parser.cpp.
bool convert<YmlSystem::Parameter>::decode(const Node& node, YmlSystem::Parameter& rhs)
{
    rhs.id = node["id"].as<std::string>();
    const Node timeDependentNode = node["time-dependent"];
    rhs.time_dependent = timeDependentNode.IsDefined() ? timeDependentNode.as<bool>() : false;
    const Node scenarioDependentNode = node["scenario-dependent"];
    rhs.scenario_dependent = scenarioDependentNode.IsDefined() ? scenarioDependentNode.as<bool>()
                                                               : false;
    rhs.value = node["value"].as<std::string>();
    return true;
}

bool convert<YmlSystem::Property>::decode(const Node& node, YmlSystem::Property& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.value = node["value"].as<std::string>();
    return true;
}

bool convert<YmlSystem::Component>::decode(const Node& node, YmlSystem::Component& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.model = node["model"].as<std::string>();
    rhs.scenarioGroup = node["scenario-group"].as<std::string>("");
    rhs.parameters = as_fallback_default<std::vector<YmlSystem::Parameter>>(node["parameters"]);
    rhs.properties = as_fallback_default<std::vector<YmlSystem::Property>>(node["properties"]);
    return true;
}

bool convert<YmlSystem::Connection>::decode(const Node& node, YmlSystem::Connection& rhs)
{
    rhs.firstEntry.componentId = node["component1"].as<std::string>();
    rhs.firstEntry.portId = node["port1"].as<std::string>();

    rhs.secondEntry.componentId = node["component2"].as<std::string>();
    rhs.secondEntry.portId = node["port2"].as<std::string>();
    return true;
}

bool convert<YmlSystem::AreaConnection>::decode(const Node& node, YmlSystem::AreaConnection& rhs)
{
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    rhs.areaId = node["area"].as<std::string>();
    return true;
}

bool convert<YmlSystem::ThermalComponent>::decode(const Node& node,
                                                  YmlSystem::ThermalComponent& rhs)
{
    rhs.areaId = node["area"].as<std::string>();
    rhs.clusterId = node["cluster-id"].as<std::string>();
    return true;
}

bool convert<YmlSystem::ThermalCapacityConnection>::decode(
  const Node& node,
  YmlSystem::ThermalCapacityConnection& rhs)
{
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    rhs.thermalComponent = node["thermal-component"].as<YmlSystem::ThermalComponent>();
    return true;
}

bool convert<YmlSystem::System>::decode(const Node& node, YmlSystem::System& rhs)
{
    rhs.id = node["id"].as<std::string>("");
    rhs.components = as_fallback_default<std::vector<YmlSystem::Component>>(node["components"]);
    rhs.connections = as_fallback_default<std::vector<YmlSystem::Connection>>(node["connections"]);
    rhs.areaConnections = as_fallback_default<std::vector<YmlSystem::AreaConnection>>(
      node["area-connections"]);
    rhs.thermalCapacityConnections = as_fallback_default<
      std::vector<YmlSystem::ThermalCapacityConnection>>(node["thermal-capacity-connections"]);
    return true;
}

} // namespace YAML
