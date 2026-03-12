
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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
    std::string location;
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
    std::string location;
    std::string out_of_bounds_processing_mode;
};

struct ExtraOutput
{
    std::string id;
    std::string expression;
};

struct Objective
{
    std::string id;
    std::string expression;
    std::string location;
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
    std::vector<Constraint> binding_constraints;
    std::vector<Objective> objectives;
    std::vector<ExtraOutput> extra_outputs;
};

struct AreaConnection
{
    std::string inject_to_balance;
    std::string spillage_bound;
    std::string unsupplied_energy_bound;
};

struct PortType
{
    std::string id;
    std::string description;
    std::vector<std::string> fields;
    std::string thermal_capacity_connection_field;
    AreaConnection area_connection;
};

struct Library
{
    std::string id;
    std::string description;
    std::vector<PortType> port_types;
    std::vector<Model> models;
};
} // namespace Antares::IO::Inputs::YmlModel
