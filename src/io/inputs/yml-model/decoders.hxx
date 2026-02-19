
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>

#include "antares/io/inputs/yml-model/Library.h"

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
 * as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(
node["parameters"]) is equivalent to
 node["parameters"].as<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(std::vector<Antares::IO::Inputs::YmlModel::Parameter>())
 */
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

template<>
struct convert<Antares::IO::Inputs::YmlModel::Parameter>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Parameter& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.time_dependent = node["time-dependent"].as<bool>(true);
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::ValueType>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::ValueType& rhs)
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
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Variable>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Variable& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.lower_bound = node["lower-bound"].as<std::string>("");
        rhs.upper_bound = node["upper-bound"].as<std::string>("");
        rhs.variable_type = node["variable-type"].as<Antares::IO::Inputs::YmlModel::ValueType>(
          Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS);
        rhs.time_dependent = node["time-dependent"].as<bool>(true);
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
        rhs.location = node["location"].as<std::string>("subproblems");
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Port>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Port& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.type = node["type"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::PortFieldDefinition>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::PortFieldDefinition& rhs)
    {
        if (!node.IsMap())
        {
            // return true to avoid error ? port field definition not mandatory
            return false;
        }
        rhs.port = node["port"].as<std::string>();
        rhs.field = node["field"].as<std::string>();
        rhs.definition = node["definition"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Constraint>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Constraint& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        rhs.location = node["location"].as<std::string>("subproblems");
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::ExtraOutput>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::ExtraOutput& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Objective>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Objective& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        rhs.location = node["location"].as<std::string>("subproblems");
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Model>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Model& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
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
        rhs.constraints = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlModel::Constraint>>(node["constraints"]);
        rhs.binding_constraints = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlModel::Constraint>>(node["binding-constraints"]);
        rhs.objectives = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Objective>>(
          node["objective-contributions"]);
        rhs.extra_outputs = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlModel::ExtraOutput>>(node["extra-outputs"]);
        return true;
    }
};

std::string getFieldFromNode(const Node& node, const std::string& fieldName)
{
    if (node[fieldName].IsNull())
    {
        return {};
    }
    return node[fieldName].as<std::string>("");
}

bool isValidMap(const Node& node, const unsigned& nbFields)
{
    if (node.IsMap() && node.size() == nbFields)
    {
        return true;
    }
    return false;
}

template<>
struct convert<Antares::IO::Inputs::YmlModel::PortType>
{
    static bool convertAreaConnectionFields(const Node& node,
                                            Antares::IO::Inputs::YmlModel::PortType& rhs)
    {
        auto child_node = node["area-connection"];
        if (!child_node.IsDefined())
        {
            return true;
        }

        const unsigned expectedNbFields = 3;
        if (!isValidMap(child_node, expectedNbFields))
        {
            return false;
        }

        rhs.area_connection.inject_to_balance = getFieldFromNode(child_node,
                                                                 "injection-to-balance");
        rhs.area_connection.spillage_bound = getFieldFromNode(child_node, "spillage-bound");
        rhs.area_connection.unsupplied_energy_bound = getFieldFromNode(child_node,
                                                                       "unsupplied-energy-bound");
        return true;
    }

    static bool convertThermalCapacityField(const Node& node,
                                            Antares::IO::Inputs::YmlModel::PortType& rhs)
    {
        auto child_node = node["thermal-capacity-connection"];
        if (!child_node.IsDefined())
        {
            return true;
        }

        const unsigned expectedNbFields = 1;
        if (!isValidMap(child_node, expectedNbFields))
        {
            return false;
        }

        rhs.thermal_capacity_connection_field = getFieldFromNode(child_node, "capacity-field");
        return true;
    }

    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::PortType& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>("");
        for (const auto& field: node["fields"])
        {
            rhs.fields.push_back(field["id"].as<std::string>());
        }
        if (!convertThermalCapacityField(node, rhs))
        {
            return false;
        }
        if (!convertAreaConnectionFields(node, rhs))
        {
            return false;
        }

        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Library>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Library& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>("");
        rhs.port_types = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::PortType>>(
          node["port-types"]);
        rhs.models = node["models"].as<std::vector<Antares::IO::Inputs::YmlModel::Model>>();
        return true;
    }
};
} // namespace YAML
