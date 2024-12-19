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

#include "antares/solver/systemParser/system.h"

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
 * as_fallback_default<std::vector<Antares::Solver::SystemParser::Parameter>>(
node["parameters"]) is equivalent to
 node["parameters"].as<std::vector<Antares::Solver::SystemParser::Parameter>>(std::vector<Antares::Solver::SystemParser::Parameter>())
 */
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

template<>
struct convert<Antares::Solver::SystemParser::Parameter>
{
    static bool decode(const Node& node, Antares::Solver::SystemParser::Parameter& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.type = node["type"].as<std::string>();
        rhs.value = node["value"].as<double>();
        return true;
    }
};

template<>
struct convert<Antares::Solver::SystemParser::Component>
{
    static bool decode(const Node& node, Antares::Solver::SystemParser::Component& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.model = node["model"].as<std::string>();
        rhs.scenarioGroup = node["scenario-group"].as<std::string>();
        rhs.parameters = as_fallback_default<std::vector<Antares::Solver::SystemParser::Parameter>>(
          node["parameters"]);
        return true;
    }
};

template<>
struct convert<Antares::Solver::SystemParser::System>
{
    static bool decode(const Node& node, Antares::Solver::SystemParser::System& rhs)
    {
        rhs.id = node["id"].as<std::string>();
        rhs.libraries = as_fallback_default<std::vector<std::string>>(node["model-libraries"]);
        rhs.components = as_fallback_default<std::vector<Antares::Solver::SystemParser::Component>>(
          node["components"]);
        return true;
    }
};

} // namespace YAML
