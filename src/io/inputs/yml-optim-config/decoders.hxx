
/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include "antares/io/inputs/yml-optim-config/OptimConfig.h"

#include "yaml-cpp/yaml.h"

// Implement convert specializations
namespace YAML
{

// TODO this function is defined at least 3 times, deduplicate
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

template<>
struct convert<Antares::IO::Inputs::YmlOptimConfig::Variable>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Variable& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.location = node["location"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlOptimConfig::Constraint>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Constraint& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.location = node["location"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlOptimConfig::Objective>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Objective& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.location = node["location"].as<std::string>();

        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlOptimConfig::Model>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Model& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        const auto& modelDecompositionNode = node["model-decomposition"];
        rhs.variables = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::Variable>>(
          modelDecompositionNode["variables"]);

        rhs.constraints = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::Constraint>>(
          modelDecompositionNode["constraints"]);

        rhs.objectives = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::Objective>>(
          modelDecompositionNode["objective-contributions"]);

        return true;
    }
};

} // namespace YAML
