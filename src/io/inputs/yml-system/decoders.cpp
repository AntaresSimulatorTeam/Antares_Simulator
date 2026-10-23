// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/decoders.h"

#include <fmt/format.h>

#include <antares/io/inputs/InputError.h>
#include <antares/io/inputs/yml-utils/YmlTreeDisplayer.h>
#include <antares/io/inputs/yml-utils/checkFields.h>

#include "include/antares/io/inputs/yml-system/system.h"

using namespace Antares::IO::Inputs;

namespace YAML
{

bool convert<YmlSystem::Parameter>::decode(const Node& node, YmlSystem::Parameter& rhs)
{
    if (!YmlUtils::requireMap(node, "parameter"))
    {
        return false;
    }
    if (!checkFields(node,
                     {"id", "value"},
                     {"time-dependent", "scenario-dependent", "scenario-group"}))
    {
        return true;
    }
    rhs.id = node["id"].as<std::string>();
    rhs.time_dependent = node["time-dependent"].as<bool>(false);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(false);
    rhs.value = node["value"].as<std::string>();
    return true;
}

bool convert<YmlSystem::Property>::decode(const Node& node, YmlSystem::Property& rhs)
{
    if (!YmlUtils::requireMap(node, "properties"))
    {
        return false;
    }
    if (!checkFields(node, {"id", "value"}))
    {
        return true;
    }
    rhs.id = node["id"].as<std::string>();
    rhs.value = node["value"].as<std::string>();
    return true;
}

bool convert<YmlSystem::Component>::decode(const Node& node, YmlSystem::Component& rhs)
{
    if (!YmlUtils::requireMap(node, "component"))
    {
        return false;
    }
    if (!checkFields(node, {"id", "model"}, {"scenario-group", "parameters", "properties"}))
    {
        return true;
    }
    rhs.id = node["id"].as<std::string>();
    rhs.model = node["model"].as<std::string>();
    rhs.scenarioGroup = node["scenario-group"].as<std::string>("");
    rhs.parameters = as_fallback_default<std::vector<YmlSystem::Parameter>>(node["parameters"]);
    rhs.properties = as_fallback_default<std::vector<YmlSystem::Property>>(node["properties"]);
    return true;
}

bool convert<YmlSystem::Connection>::decode(const Node& node, YmlSystem::Connection& rhs)
{
    if (!YmlUtils::requireMap(node, "connection"))
    {
        return false;
    }
    if (!checkFields(node, {"component1", "port1", "component2", "port2"}))
    {
        return true;
    }
    rhs.firstEntry.componentId = node["component1"].as<std::string>();
    rhs.firstEntry.portId = node["port1"].as<std::string>();

    rhs.secondEntry.componentId = node["component2"].as<std::string>();
    rhs.secondEntry.portId = node["port2"].as<std::string>();
    return true;
}

bool convert<YmlSystem::AreaConnection>::decode(const Node& node, YmlSystem::AreaConnection& rhs)
{
    if (!YmlUtils::requireMap(node, "area-connection"))
    {
        return false;
    }
    if (!checkFields(node, {"component", "port", "area"}))
    {
        return true;
    }
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    rhs.areaId = node["area"].as<std::string>();
    return true;
}

bool convert<YmlSystem::ThermalComponent>::decode(const Node& node,
                                                  YmlSystem::ThermalComponent& rhs)
{
    if (!YmlUtils::requireMap(node, "thermal-component"))
    {
        return false;
    }
    if (!checkFields(node, {"area", "cluster-id"}))
    {
        return true;
    }
    rhs.areaId = node["area"].as<std::string>();
    rhs.clusterId = node["cluster-id"].as<std::string>();
    return true;
}

bool convert<YmlSystem::ThermalCapacityConnection>::decode(
  const Node& node,
  YmlSystem::ThermalCapacityConnection& rhs)
{
    if (!YmlUtils::requireMap(node, "thermal-capacity-connection"))
    {
        return false;
    }
    if (!checkFields(node, {"component", "port", "thermal-component"}))
    {
        return true;
    }
    rhs.componentId = node["component"].as<std::string>();
    rhs.portId = node["port"].as<std::string>();
    const Node thermalComponentNode = node["thermal-component"];
    if (!thermalComponentNode.IsDefined())
    {
        YmlTreeDisplayer displayer(node);
        throw InputError(fmt::format("Missing required field 'thermal-component'.\n{}{}",
                                     displayer.baseTree(),
                                     displayer.buildMarkedTree({}, {"thermal-component"})));
    }
    if (thermalComponentNode.IsNull())
    {
        YmlTreeDisplayer displayer(node);
        throw InputError(
          fmt::format("Expected a YAML mapping for 'thermal-component' (got null).\n{}{}",
                      displayer.baseTree(),
                      displayer.buildMarkedTree({"thermal-component"}, {})));
    }
    rhs.thermalComponent = thermalComponentNode.as<YmlSystem::ThermalComponent>();
    return true;
}

bool convert<YmlSystem::System>::decode(const Node& node, YmlSystem::System& rhs)
{
    if (!node.IsMap())
    {
        throw InputError("Expected a YAML mapping for 'system'");
    }
    if (!checkFields(node,
                {},
                {"id",
                 "description",
                 "model-libraries",
                 "components",
                 "connections",
                 "area-connections",
                 "thermal-capacity-connections"}))
    {
        return true;
    }
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
