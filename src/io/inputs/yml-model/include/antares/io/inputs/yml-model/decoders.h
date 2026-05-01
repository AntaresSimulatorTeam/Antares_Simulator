// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <fmt/format.h>
#include <unordered_set>

#include "antares/io/inputs/yml-model/Library.h"
#include "antares/io/inputs/yml-utils/YmlTreeDisplayer.h"

// Implement convert specializations
namespace YAML
{

/**
 * @brief shortened to default construct a value when node is null
 * @tparam T Type to convert the node to
 * @param n node
 * @return Object of type T
 * It's just to simplify repetitive and verbose lines
 * as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(
node["parameters"]) is equivalent to
 node["parameters"].as<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(std::vector<Antares::IO::Inputs::YmlModel::Parameter>())
 */
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

void checkMandatoryIdField(const Node& node, const std::string& nodeName);

/// Returns the string value of a field from a YAML node, or empty string if absent/null.
std::string getFieldFromNode(const Node& node, const std::string& fieldName);

void checkFields(const Node& node, const std::unordered_set<std::string>& allowedFields);

template<>
class convert<Antares::IO::Inputs::YmlModel::PortType>
{
public:
    static bool convertAreaConnectionFields(const Node& node,
                                            Antares::IO::Inputs::YmlModel::PortType& rhs);
    static bool convertThermalCapacityField(const Node& node,
                                            Antares::IO::Inputs::YmlModel::PortType& rhs);
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::PortType& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Library>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Library& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Parameter>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Parameter& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::ValueType>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::ValueType& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Variable>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Variable& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Port>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Port& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::PortFieldDefinition>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::PortFieldDefinition& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Constraint>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Constraint& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::ExtraOutput>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::ExtraOutput& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Objective>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Objective& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlModel::Model>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Model& rhs);
};
} // namespace YAML
