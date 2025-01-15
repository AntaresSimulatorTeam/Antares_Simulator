
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

#include "antares/solver/modelParser/Library.h"

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
 * as_fallback_default<std::vector<Antares::Solver::ModelParser::Parameter>>(
node["parameters"]) is equivalent to
 node["parameters"].as<std::vector<Antares::Solver::ModelParser::Parameter>>(std::vector<Antares::Solver::ModelParser::Parameter>())
 */
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

template<>
struct convert<Antares::Solver::ModelParser::Parameter>
{
    static bool decode(const Node& node, Antares::Solver::ModelParser::Parameter& rhs)
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
struct convert<Antares::Solver::ModelParser::ValueType>
{
    static bool decode(const Node& node, Antares::Solver::ModelParser::ValueType& rhs)
    {
        if (!node.IsScalar())
        {
            return false;
        }
        const auto value = node.as<std::string>();
        if (value == "continuous")
        {
            rhs = Antares::Solver::ModelParser::ValueType::CONTINUOUS;
        }
        else if (value == "integer")
        {
            rhs = Antares::Solver::ModelParser::ValueType::INTEGER;
        }
        else if (value == "boolean")
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
    static bool decode(const Node& node, Antares::Solver::ModelParser::Variable& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.lower_bound = node["lower-bound"].as<std::string>("");
        rhs.upper_bound = node["upper-bound"].as<std::string>("");
        rhs.variable_type = node["variable-type"].as<Antares::Solver::ModelParser::ValueType>(
          Antares::Solver::ModelParser::ValueType::CONTINUOUS);
        rhs.time_dependent = node["time-dependent"].as<bool>(true);
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Port>
{
    static bool decode(const Node& node, Antares::Solver::ModelParser::Port& rhs)
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
struct convert<Antares::Solver::ModelParser::PortFieldDefinition>
{
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
    static bool decode(const Node& node, Antares::Solver::ModelParser::Constraint& rhs)
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
struct convert<Antares::Solver::ModelParser::Model>
{
    static bool decode(const Node& node, Antares::Solver::ModelParser::Model& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>("");
        rhs.parameters = as_fallback_default<std::vector<Antares::Solver::ModelParser::Parameter>>(
          node["parameters"]);
        rhs.variables = as_fallback_default<std::vector<Antares::Solver::ModelParser::Variable>>(
          node["variables"]);
        rhs.ports = as_fallback_default<std::vector<Antares::Solver::ModelParser::Port>>(
          node["ports"]);
        rhs.port_field_definitions = as_fallback_default<
          std::vector<Antares::Solver::ModelParser::PortFieldDefinition>>(
          node["port-field-definitions"]);
        rhs.constraints = as_fallback_default<
          std::vector<Antares::Solver::ModelParser::Constraint>>(node["constraints"]);
        rhs.objective = node["objective"].as<std::string>("");
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::PortType>
{
    static bool decode(const Node& node, Antares::Solver::ModelParser::PortType& rhs)
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
        return true;
    }
};

template<>
struct convert<Antares::Solver::ModelParser::Library>
{
    static bool decode(const Node& node, Antares::Solver::ModelParser::Library& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>("");
        rhs.port_types = as_fallback_default<std::vector<Antares::Solver::ModelParser::PortType>>(
          node["port-types"]);
        rhs.models = node["models"].as<std::vector<Antares::Solver::ModelParser::Model>>();
        return true;
    }
};
} // namespace YAML
