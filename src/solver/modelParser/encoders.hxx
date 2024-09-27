
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

#include "antares/solver/modelParser/model.h"

#include "yaml-cpp/yaml.h"

// Implement convert specializations
namespace YAML
{
template<>
struct convert<Antares::Solver::ModelParser::Parameter>
{
    static Node encode(const Antares::Solver::ModelParser::Parameter& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["time-dependent"] = rhs.time_dependent;
        node["scenario-dependent"] = rhs.scenario_dependent;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::Parameter& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.name = node["name"].as<std::string>();
        rhs.time_dependent = node["time-dependent"].as<bool>();
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>();
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::ValueType>
{
    static Node encode(const Antares::Solver::ModelParser::ValueType& rhs)
    {
        Node node;
        node = rhs == Antares::Solver::ModelParser::ValueType::FLOAT     ? "FLOAT"
               : rhs == Antares::Solver::ModelParser::ValueType::INTEGER ? "INTEGER"
                                                                         : "BOOL";
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::ValueType& rhs)
    {
        if (!node.IsScalar())
        {
            return false;
        }
        if (node.as<std::string>() == "FLOAT")
        {
            rhs = Antares::Solver::ModelParser::ValueType::FLOAT;
        }
        else if (node.as<std::string>() == "INTEGER")
        {
            rhs = Antares::Solver::ModelParser::ValueType::INTEGER;
        }
        else if (node.as<std::string>() == "BOOL")
        {
            rhs = Antares::Solver::ModelParser::ValueType::BOOL;
        }
        else
        {
            return false;
        }
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Variable>
{
    static Node encode(const Antares::Solver::ModelParser::Variable& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["lower-bound"] = rhs.lower_bound;
        node["upper-bound"] = rhs.upper_bound;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::Variable& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.name = node["name"].as<std::string>();
        rhs.lower_bound = node["lower-bound"].as<std::string>();
        rhs.upper_bound = node["upper-bound"].as<std::string>();
        rhs.variable_type = node["variable-type"].as<Antares::Solver::ModelParser::ValueType>(
          Antares::Solver::ModelParser::ValueType::FLOAT);
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Port>
{
    static Node encode(const Antares::Solver::ModelParser::Port& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["type"] = rhs.type;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::Port& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.name = node["name"].as<std::string>();
        rhs.type = node["type"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::PortFieldDefinition>
{
    static Node encode(const Antares::Solver::ModelParser::PortFieldDefinition& rhs)
    {
        Node node;
        node["port"] = rhs.port;
        node["field"] = rhs.field;
        node["definition"] = rhs.definition;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::PortFieldDefinition& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.port = node["port"].as<std::string>();
        rhs.field = node["field"].as<std::string>();
        rhs.definition = node["definition"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Constraint>
{
    static Node encode(const Antares::Solver::ModelParser::Constraint& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["expression"] = rhs.expression;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::Constraint& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.name = node["name"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Model>
{
    static Node encode(const Antares::Solver::ModelParser::Model& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["parameters"] = rhs.parameters;
        node["variables"] = rhs.variables;
        node["ports"] = rhs.ports;
        node["port-field-definitions"] = rhs.port_field_definitions;
        node["constraints"] = rhs.constraints;
        node["objective"] = rhs.objective;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::Model& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        rhs.parameters = node["parameters"]
                           .as<std::vector<Antares::Solver::ModelParser::Parameter>>();
        rhs.variables = node["variables"].as<std::vector<Antares::Solver::ModelParser::Variable>>();
        rhs.ports = node["ports"].as<std::vector<Antares::Solver::ModelParser::Port>>();
        rhs.port_field_definitions = node["port-field-definitions"]
                                       .as<std::vector<
                                         Antares::Solver::ModelParser::PortFieldDefinition>>();
        rhs.constraints = node["constraints"]
                            .as<std::vector<Antares::Solver::ModelParser::Constraint>>();
        rhs.objective = node["objective"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::PortType>
{
    static Node encode(const Antares::Solver::ModelParser::PortType& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["fields"] = rhs.fields;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::PortType& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        for (const auto& field: node["fields"])
        {
            rhs.fields.push_back(field["name"].as<std::string>());
        }
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Library>
{
    static Node encode(const Antares::Solver::ModelParser::Library& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["port-types"] = rhs.port_types;
        node["models"] = rhs.models;
        return node;
    }

    static bool decode(const Node& node, Antares::Solver::ModelParser::Library& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        rhs.port_types = node["port-types"]
                           .as<std::vector<Antares::Solver::ModelParser::PortType>>();
        rhs.models = node["models"].as<std::vector<Antares::Solver::ModelParser::Model>>();
        return true;
    }
};
} // namespace YAML
