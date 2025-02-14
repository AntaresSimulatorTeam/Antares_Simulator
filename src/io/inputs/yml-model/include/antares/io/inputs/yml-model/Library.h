
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
#include <iostream>
#include <string>
#include <vector>

namespace Antares::IO::Inputs::YmlModel
{
// Define structures
struct Parameter
{
    std::string id;
    bool time_dependent;
    bool scenario_dependent;
};

enum class ValueType
{
    CONTINUOUS,
    INTEGER,
    BOOL
};

inline std::string toString(const ValueType& value_type)
{
    using namespace std::string_literals;
    switch (value_type)
    {
    case ValueType::CONTINUOUS:
        return "CONTINUOUS"s;
    case ValueType::INTEGER:
        return "INTEGER"s;
    case ValueType::BOOL:
        return "BOOL"s;
    default:
        return "UNKNOWN"s;
    }
}

struct Variable
{
    std::string id;
    std::string lower_bound;
    std::string upper_bound;
    ValueType variable_type;
    bool time_dependent;
    bool scenario_dependent;
};

struct Port
{
    std::string id;
    std::string type;
};

struct PortFieldDefinition
{
    std::string port;
    std::string field;
    std::string definition;
};

struct Constraint
{
    std::string id;
    std::string expression;
};

struct Model
{
    std::string id;
    std::string description;
    std::vector<Parameter> parameters;
    std::vector<Variable> variables;
    std::vector<Port> ports;
    std::vector<PortFieldDefinition> port_field_definitions;
    std::vector<Constraint> constraints;
    std::string objective;
};

struct PortType
{
    std::string id;
    std::string description;
    // Small optimization: we only need the name of the fields
    // No need for an intermediate struct "field" with just a string "name" member
    std::vector<std::string> fields;
};

struct Library
{
    std::string id;
    std::string description;
    std::vector<PortType> port_types;
    std::vector<Model> models;
};
} // namespace Antares::IO::Inputs::YmlModel
