
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
#include "yaml-cpp/yaml.h"

struct Library;
struct Model;
struct Parameter;

struct Library
{
    std::string id;
    std::string description;
    std::vector<Model> models;
};

struct Model
{
    std::string id;
    std::string description;
    std::vector<Parameter> parameters;
    std::string objective;
};

struct Parameter
{
    std::string name;
    bool time_dependent;
    bool scenario_dependent;
};

namespace YAML
{
template<>
struct convert<Library>
{
    static Node encode(const Library& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["models"] = rhs.models;
        return node;
    }

    static bool decode(const Node& node, Library& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }

        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        rhs.models = node["models"].as<std::vector<Model>>();
        return true;
    }
};

// Model
template<>
struct convert<Model>
{
    static Node encode(const Model& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["description"] = rhs.description;
        node["parameters"] = rhs.parameters;
        node["objective"] = rhs.objective;
        return node;
    }

    static bool decode(const Node& node, Model& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }

        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>();
        rhs.parameters = node["parameters"].as<std::vector<Parameter>>();
        rhs.objective = node["objective"].as<std::string>();
        return true;
    }
};

// Parameter
template<>
struct convert<Parameter>
{
    static Node encode(const Parameter& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["time-dependent"] = rhs.time_dependent;
        node["scenario-dependent"] = rhs.scenario_dependent;
        return node;
    }

    static bool decode(const Node& node, Parameter& rhs)
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
} // namespace YAML
