// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/decoders.h"

#include <antares/io/inputs/InputError.h>

#include "include/antares/io/inputs/yml-model/Library.h"

using namespace Antares::IO::Inputs;

namespace YAML
{

// Keep field reads in sync with the validation schema in parser.cpp.
std::string getFieldFromNode(const Node& node, const std::string& fieldName)
{
    if (!node[fieldName].IsDefined() || node[fieldName].IsNull())
    {
        return {};
    }
    return node[fieldName].as<std::string>("");
}

bool convert<YmlModel::ExpressionLineNumber>::decode(const Node& node,
                                                     YmlModel::ExpressionLineNumber& rhs)
{
    rhs.line_number = 0;
    if (node.IsDefined() && !node.IsNull())
    {
        rhs.line_number = node.Mark().line + 1;
    }
    rhs.input_expr = node.as<std::string>("");
    return true;
}

void convert<YmlModel::PortType>::convertAreaConnectionFields(const Node& node,
                                                              YmlModel::PortType& rhs)
{
    const auto areaConnNode = node["area-connection"];

    if (!areaConnNode.IsDefined())
    {
        return;
    }

    rhs.area_connection.inject_to_balance = getFieldFromNode(areaConnNode, "injection-to-balance");
    rhs.area_connection.spillage_bound = getFieldFromNode(areaConnNode, "spillage-bound");
    rhs.area_connection.unsupplied_energy_bound = getFieldFromNode(areaConnNode,
                                                                   "unsupplied-energy-bound");
    rhs.area_connection.price = getFieldFromNode(areaConnNode, "price");
}

void convert<YmlModel::PortType>::convertThermalCapacityField(const Node& node,
                                                              YmlModel::PortType& rhs)
{
    auto childNode = node["thermal-capacity-connection"];
    if (!childNode.IsDefined())
    {
        return;
    }

    rhs.thermal_capacity_connection_field = getFieldFromNode(childNode, "capacity-field");
}

void convert<YmlModel::PortType>::convertLinkCapacityFields(const Node& node,
                                                            YmlModel::PortType& rhs)
{
    auto childNode = node["link-capacity-connection"];
    if (!childNode.IsDefined())
    {
        return;
    }

    rhs.link_capacity_connection_direct_field = getFieldFromNode(childNode,
                                                                 "capacity-direct-field");
    rhs.link_capacity_connection_indirect_field = getFieldFromNode(childNode,
                                                                   "capacity-indirect-field");
}

bool convert<YmlModel::PortType>::decode(const Node& node, YmlModel::PortType& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    for (const auto& field: node["fields"])
    {
        rhs.fields.push_back(field["id"].as<std::string>());
    }

    convertAreaConnectionFields(node, rhs);
    convertThermalCapacityField(node, rhs);
    convertLinkCapacityFields(node, rhs);
    return true;
}

bool convert<YmlModel::Parameter>::decode(const Node& node, YmlModel::Parameter& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.time_dependent = node["time-dependent"].as<bool>(true);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
    return true;
}

bool convert<YmlModel::ValueType>::decode(const Node& node, YmlModel::ValueType& rhs)
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
        throw InputError("Unknown variable-type: '" + value
                         + "'. Expected one of: 'continuous', 'integer', 'boolean'");
    }
    return true;
}

bool convert<YmlModel::Variable>::decode(const Node& node, YmlModel::Variable& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.lower_bound = node["lower-bound"].as<YmlModel::ExpressionLineNumber>(
      YmlModel::ExpressionLineNumber{});
    // optional key
    rhs.upper_bound = node["upper-bound"].as<YmlModel::ExpressionLineNumber>(
      YmlModel::ExpressionLineNumber{});
    // optional key
    rhs.variable_type = node["variable-type"].as<YmlModel::ValueType>(
      YmlModel::ValueType::CONTINUOUS);
    rhs.time_dependent = node["time-dependent"].as<bool>(true);
    rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<YmlModel::Port>::decode(const Node& node, YmlModel::Port& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.type = node["type"].as<std::string>();
    return true;
}

bool convert<YmlModel::PortFieldDefinition>::decode(const Node& node,
                                                    YmlModel::PortFieldDefinition& rhs)
{
    rhs.port = node["port"].as<std::string>();
    rhs.field = node["field"].as<std::string>();
    rhs.definition = node["definition"].as<YmlModel::ExpressionLineNumber>();
    return true;
}

bool convert<YmlModel::Constraint>::decode(const Node& node, YmlModel::Constraint& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.expression = node["expression"].as<YmlModel::ExpressionLineNumber>();
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<YmlModel::ExtraOutput>::decode(const Node& node, YmlModel::ExtraOutput& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.expression = node["expression"].as<YmlModel::ExpressionLineNumber>();
    return true;
}

bool convert<YmlModel::Objective>::decode(const Node& node, YmlModel::Objective& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.expression = node["expression"].as<YmlModel::ExpressionLineNumber>();
    rhs.location = node["location"].as<std::string>("subproblems");
    return true;
}

bool convert<YmlModel::Model>::decode(const Node& node, YmlModel::Model& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    rhs.parameters = as_fallback_default<std::vector<YmlModel::Parameter>>(node["parameters"]);
    rhs.variables = as_fallback_default<std::vector<YmlModel::Variable>>(node["variables"]);
    rhs.ports = as_fallback_default<std::vector<YmlModel::Port>>(node["ports"]);
    rhs.port_field_definitions = as_fallback_default<std::vector<YmlModel::PortFieldDefinition>>(
      node["port-field-definitions"]);
    rhs.constraints = as_fallback_default<std::vector<YmlModel::Constraint>>(node["constraints"]);
    rhs.binding_constraints = as_fallback_default<std::vector<YmlModel::Constraint>>(
      node["binding-constraints"]);
    rhs.objectives = as_fallback_default<std::vector<YmlModel::Objective>>(
      node["objective-contributions"]);
    rhs.extra_outputs = as_fallback_default<std::vector<YmlModel::ExtraOutput>>(
      node["extra-outputs"]);
    return true;
}

bool convert<YmlModel::Library>::decode(const Node& node, YmlModel::Library& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    rhs.port_types = as_fallback_default<std::vector<YmlModel::PortType>>(node["port-types"]);
    if (node["models"].IsDefined() && !node["models"].IsNull())
    {
        rhs.models = node["models"].as<std::vector<YmlModel::Model>>();
    }
    return true;
}

} // namespace YAML
