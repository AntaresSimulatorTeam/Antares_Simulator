// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/decoders.h"

#include <unordered_set>

#include <antares/io/inputs/InputError.h>

using namespace Antares::IO::Inputs;

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
        throw InputError(std::string("Expected a YAML mapping for '")
                                              + typeName + "'");
    }
    return false;
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
                                      YmlUtils::printPathTree(nodePath)));
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

bool convert<YmlModel::PortType>::convertAreaConnectionFields(
  const Node& node,
  YmlModel::PortType& rhs)
{
    auto areaConnNode = node["area-connection"];
    if (!areaConnNode.IsDefined())
    {
        return true;
    }

    if (!areaConnNode.IsMap())
    {
        throw InputError(
          "Expected a YAML mapping for 'area-connection' in port-type");
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

bool convert<YmlModel::PortType>::convertThermalCapacityField(
  const Node& node,
  YmlModel::PortType& rhs)
{
    auto childNode = node["thermal-capacity-connection"];
    if (!childNode.IsDefined())
    {
        return true;
    }

    if (!childNode.IsMap())
    {
        throw InputError(
          "Expected a YAML mapping for 'thermal-capacity-connection' in port-type");
    }

    checkFields(childNode, std::unordered_set<std::string>{"capacity-field"});

    rhs.thermal_capacity_connection_field = getFieldFromNode(childNode, "capacity-field");
    return true;
}

bool convert<YmlModel::PortType>::decode(
  const Node& node,
  YmlModel::PortType& rhs)
{
    if (!requireMap(node, "port-type"))
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

bool convert<YmlModel::Parameter>::decode(
  const Node& node,
  YmlModel::Parameter& rhs)
{
    if (!requireMap(node, "parameter"))
    {
        return false;
    }

    rhs.time_dependent = node["time-dependent"].as<bool>(true);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
    return true;
}

bool convert<YmlModel::ValueType>::decode(
  const Node& node,
  YmlModel::ValueType& rhs)
{
    if (!node.IsScalar())
    {
        if (node.IsDefined() && !node.IsNull())
        {
            throw InputError("Expected a scalar value for 'variable-type'");
        }
        return false;
    }
    const auto value = node.as<std::string>();
    if (value == "continuous")
    {
        rhs = YmlModel::ValueType::CONTINUOUS;
    }
    else if (value == "integer")
    {
        rhs = YmlModel::ValueType::INTEGER;
    }
    else if (value == "boolean")
    {
        rhs = YmlModel::ValueType::BOOL;
    }
    else
    {
        throw InputError(
          "Unknown variable-type: '" + value
          + "'. Expected one of: 'continuous', 'integer', 'boolean'");
    }
    return true;
}

bool convert<YmlModel::Variable>::decode(
  const Node& node,
  YmlModel::Variable& rhs)
{
    if (!requireMap(node, "variable"))
    {
        return false;
    }

    rhs.lower_bound = node["lower-bound"].as<std::string>("");
    rhs.upper_bound = node["upper-bound"].as<std::string>("");
    rhs.variable_type = node["variable-type"].as<YmlModel::ValueType>(
      YmlModel::ValueType::CONTINUOUS);
    rhs.time_dependent = node["time-dependent"].as<bool>(true);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<YmlModel::Port>::decode(const Node& node,
                                                          YmlModel::Port& rhs)
{
    if (!requireMap(node, "port"))
    {
        return false;
    }

    rhs.type = node["type"].as<std::string>();
    return true;
}

bool convert<YmlModel::PortFieldDefinition>::decode(
  const Node& node,
  YmlModel::PortFieldDefinition& rhs)
{
    if (!requireMap(node, "port-field-definition"))
    {
        return false;
    }
    rhs.port = node["port"].as<std::string>();
    rhs.field = node["field"].as<std::string>();
    rhs.definition = node["definition"].as<std::string>();
    return true;
}

bool convert<YmlModel::Constraint>::decode(
  const Node& node,
  YmlModel::Constraint& rhs)
{
    if (!requireMap(node, "constraint"))
    {
        return false;
    }

    rhs.expression = node["expression"].as<std::string>();
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<YmlModel::ExtraOutput>::decode(
  const Node& node,
  YmlModel::ExtraOutput& rhs)
{
    if (!requireMap(node, "extra-output"))
    {
        return false;
    }

    rhs.expression = node["expression"].as<std::string>();
    return true;
}

bool convert<YmlModel::Objective>::decode(
  const Node& node,
  YmlModel::Objective& rhs)
{
    if (!requireMap(node, "objective"))
    {
        return false;
    }

    rhs.expression = node["expression"].as<std::string>();
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<YmlModel::Model>::decode(
  const Node& node,
  YmlModel::Model& rhs)
{
    if (!requireMap(node, "model"))
    {
        throw InputError("Expected a YAML mapping for 'model'");
    }

    rhs.description = node["description"].as<std::string>("");
    rhs.parameters = as_fallback_default<std::vector<YmlModel::Parameter>>(
      node["parameters"]);
    rhs.variables = as_fallback_default<std::vector<YmlModel::Variable>>(
      node["variables"]);
    rhs.ports = as_fallback_default<std::vector<YmlModel::Port>>(
      node["ports"]);
    rhs.port_field_definitions = as_fallback_default<
      std::vector<YmlModel::PortFieldDefinition>>(
      node["port-field-definitions"]);
    rhs.constraints = as_fallback_default<std::vector<YmlModel::Constraint>>(
      node["constraints"]);
    rhs.binding_constraints = as_fallback_default<
      std::vector<YmlModel::Constraint>>(node["binding-constraints"]);
    rhs.objectives = as_fallback_default<std::vector<YmlModel::Objective>>(
      node["objective-contributions"]);
    rhs.extra_outputs = as_fallback_default<
      std::vector<YmlModel::ExtraOutput>>(node["extra-outputs"]);
    return true;
}

bool convert<YmlModel::Library>::decode(
  const Node& node,
  YmlModel::Library& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    rhs.port_types = as_fallback_default<std::vector<YmlModel::PortType>>(
      node["port-types"]);
    rhs.models = node["models"].as<std::vector<YmlModel::Model>>();
    return true;
}

} // namespace YAML
