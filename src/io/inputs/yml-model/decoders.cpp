// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/decoders.h"

#include <unordered_set>

namespace YAML
{

namespace
{
template<typename T>
bool requireMapNodeWithId(const Node& node, const std::string& nodeName, T& rhs)
{
    if (!node.IsMap())
    {
        return false;
    }

    checkMandatoryIdField(node, nodeName);
    rhs.id = node["id"].as<std::string>();
    return true;
}

} // namespace

void checkMandatoryIdField(const Node& node, const std::string& nodeName)
{
    if (!node["id"].IsDefined() || node["id"].IsNull())
    {
        std::filesystem::path nodePath(node.Tag());
        throw KeyNotFound(node.Mark(),
                          fmt::format("{} id is mandatory in library\n{}",
                                      nodeName,
                                      Antares::IO::Inputs::YmlUtils::printPathTree(nodePath)));
    }
}

std::string getFieldFromNode(const Node& node, const std::string& fieldName)
{
    if (!node[fieldName].IsDefined() || node[fieldName].IsNull())
    {
        return {};
    }
    return node[fieldName].as<std::string>("");
}

std::optional<YmlMapMarker> checkKeysIfMap(const Node& node,
                                           const std::unordered_set<std::string>& allowedFields)
{
    // Only validate maps here; callers must ensure node is a map before calling when necessary.
    if (!node.IsMap())
    {
        return std::nullopt;
    }

    YmlMapMarker marker(node);

    // If allowedFields provided, check equality of key sets
    if (!allowedFields.empty())
    {
        if (marker.actualSet() == allowedFields)
        {
            return std::nullopt; // valid
        }

        // invalid map: return marker so caller can build an error message
        return std::make_optional<YmlMapMarker>(marker);
    }

    // invalid size: return marker so caller can build an error message
    return std::make_optional<YmlMapMarker>(marker);
}

void checkFields(const Node& node, const std::unordered_set<std::string>& allowedFields)
{
    // Validate map: if invalid, get marker and build an error message to throw
    if (auto maybeMarker = checkKeysIfMap(node, allowedFields))
    {
        const auto& marker = *maybeMarker;
        // compute unexpected = actual - allowed
        auto diffSet = [](const std::unordered_set<std::string>& setA,
                          const std::unordered_set<std::string>& setB)
        {
            std::vector<std::string> diff;
            for (const auto& item: setA)
            {
                if (setB.find(item) == setB.end())
                {
                    diff.push_back(item);
                }
            }
            return diff;
        };
        const auto unexpected = diffSet(marker.actualSet(), allowedFields);
        const auto missing = diffSet(allowedFields, marker.actualSet());

        const std::string markedFieldsTree = marker.buildMarkedTreeForUnexpectedAndMissing(
          unexpected,
          missing);

        // Build a readable list of errors (one per line), then append the tree
        std::string errors_list;
        for (const auto& f: unexpected)
        {
            errors_list += fmt::format("- Unexpected field: {}\n", f);
        }
        for (const auto& f: missing)
        {
            errors_list += fmt::format("- Missing field: {}\n", f);
        }

        // Final message: brief header, individual errors, then the tree
        const std::string message = fmt::format(
          "Unexpected or missing field(s) (expected {} field(s)).\n{}\n{}{}",
          allowedFields.size(),
          errors_list,
          marker.baseTree(),
          markedFieldsTree);

        throw Exception(node.Mark(), message);
    }
}

bool convert<Antares::IO::Inputs::YmlModel::PortType>::convertAreaConnectionFields(
  const Node& node,
  Antares::IO::Inputs::YmlModel::PortType& rhs)
{
    auto areaConnNode = node["area-connection"];
    if (!areaConnNode.IsDefined())
    {
        return true;
    }

    if (!areaConnNode.IsMap())
    {
        return false;
    }

    checkFields(areaConnNode,
                std::unordered_set<std::string>{"injection-to-balance",
                                                "spillage-bound",
                                                "unsupplied-energy-bound"});

    rhs.area_connection.inject_to_balance = getFieldFromNode(areaConnNode, "injection-to-balance");
    rhs.area_connection.spillage_bound = getFieldFromNode(areaConnNode, "spillage-bound");
    rhs.area_connection.unsupplied_energy_bound = getFieldFromNode(areaConnNode,
                                                                   "unsupplied-energy-bound");
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::PortType>::convertThermalCapacityField(
  const Node& node,
  Antares::IO::Inputs::YmlModel::PortType& rhs)
{
    auto childNode = node["thermal-capacity-connection"];
    if (!childNode.IsDefined())
    {
        return true;
    }

    if (!childNode.IsMap())
    {
        return false;
    }

    checkFields(childNode, std::unordered_set<std::string>{"capacity-field"});

    rhs.thermal_capacity_connection_field = getFieldFromNode(childNode, "capacity-field");
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::PortType>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::PortType& rhs)
{
    if (!requireMapNodeWithId(node, "port-type", rhs))
    {
        return false;
    }

    rhs.description = node["description"].as<std::string>("");
    for (const auto& field: node["fields"])
    {
        rhs.fields.push_back(field["id"].as<std::string>());
    }

    return convertThermalCapacityField(node, rhs) && convertAreaConnectionFields(node, rhs);
}

bool convert<Antares::IO::Inputs::YmlModel::Parameter>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Parameter& rhs)
{
    if (!requireMapNodeWithId(node, "parameter", rhs))
    {
        return false;
    }

    rhs.time_dependent = node["time-dependent"].as<bool>(true);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::ValueType>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::ValueType& rhs)
{
    if (!node.IsScalar())
    {
        return false;
    }
    const auto value = node.as<std::string>();
    if (value == "continuous")
    {
        rhs = Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS;
    }
    else if (value == "integer")
    {
        rhs = Antares::IO::Inputs::YmlModel::ValueType::INTEGER;
    }
    else if (value == "boolean")
    {
        rhs = Antares::IO::Inputs::YmlModel::ValueType::BOOL;
    }
    else
    {
        return false;
    }
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::Variable>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Variable& rhs)
{
    if (!requireMapNodeWithId(node, "variable", rhs))
    {
        return false;
    }

    rhs.lower_bound = node["lower-bound"].as<std::string>("");
    rhs.upper_bound = node["upper-bound"].as<std::string>("");
    rhs.variable_type = node["variable-type"].as<Antares::IO::Inputs::YmlModel::ValueType>(
      Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS);
    rhs.time_dependent = node["time-dependent"].as<bool>(true);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::Port>::decode(const Node& node,
                                                          Antares::IO::Inputs::YmlModel::Port& rhs)
{
    if (!requireMapNodeWithId(node, "Port", rhs))
    {
        return false;
    }

    rhs.type = node["type"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::PortFieldDefinition>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::PortFieldDefinition& rhs)
{
    if (!node.IsMap())
    {
        // port field definition not mandatory
        return false;
    }
    rhs.port = node["port"].as<std::string>();
    rhs.field = node["field"].as<std::string>();
    rhs.definition = node["definition"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::Constraint>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Constraint& rhs)
{
    if (!requireMapNodeWithId(node, "constraint", rhs))
    {
        return false;
    }

    rhs.expression = node["expression"].as<std::string>();
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::ExtraOutput>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::ExtraOutput& rhs)
{
    if (!requireMapNodeWithId(node, "extra-output", rhs))
    {
        return false;
    }

    rhs.expression = node["expression"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::Objective>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Objective& rhs)
{
    if (!requireMapNodeWithId(node, "objective", rhs))
    {
        return false;
    }

    rhs.expression = node["expression"].as<std::string>();
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::Model>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Model& rhs)
{
    if (!requireMapNodeWithId(node, "model", rhs))
    {
        return false;
    }

    rhs.description = node["description"].as<std::string>("");
    rhs.parameters = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(
      node["parameters"]);
    rhs.variables = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Variable>>(
      node["variables"]);
    rhs.ports = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Port>>(
      node["ports"]);
    rhs.port_field_definitions = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlModel::PortFieldDefinition>>(
      node["port-field-definitions"]);
    rhs.constraints = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Constraint>>(
      node["constraints"]);
    rhs.binding_constraints = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlModel::Constraint>>(node["binding-constraints"]);
    rhs.objectives = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Objective>>(
      node["objective-contributions"]);
    rhs.extra_outputs = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlModel::ExtraOutput>>(node["extra-outputs"]);
    return true;
}

bool convert<Antares::IO::Inputs::YmlModel::Library>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Library& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    rhs.port_types = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::PortType>>(
      node["port-types"]);
    rhs.models = node["models"].as<std::vector<Antares::IO::Inputs::YmlModel::Model>>();
    return true;
}

} // namespace YAML
