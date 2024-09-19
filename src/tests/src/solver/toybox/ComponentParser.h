
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

struct ComponentParameter
{
    std::string name;
    double value;
};

struct Component
{
    std::string id;
    std::string model;
    std::vector<ComponentParameter> parameters;
};

namespace YAML
{
// ComponentParameter
template<>
struct convert<ComponentParameter>
{
    static Node encode(const ComponentParameter& rhs)
    {
        Node node;
        node["name"] = rhs.name;
        node["value"] = rhs.value;
        return node;
    }

    static bool decode(const Node& node, ComponentParameter& rhs)
    {
        if (!node.IsMap() || node.size() != 2)
        {
            return false;
        }

        rhs.name = node["name"].as<std::string>();
        rhs.value = node["value"].as<double>();
        return true;
    }
};

// Component
template<>
struct convert<Component>
{
    static Node encode(const Component& rhs)
    {
        Node node;
        node["id"] = rhs.id;
        node["model"] = rhs.model;
        node["parameters"] = rhs.parameters;
        return node;
    }

    static bool decode(const Node& node, Component& rhs)
    {
        if (!node.IsMap() || node.size() != 3)
        {
            return false;
        }

        rhs.id = node["id"].as<std::string>();
        rhs.model = node["model"].as<std::string>();
        rhs.parameters = node["parameters"].as<std::vector<ComponentParameter>>();
        return true;
    }
};
} // namespace YAML
