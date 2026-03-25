
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/io/inputs/yml-optim-config/OptimConfig.h"

#include "yaml-cpp/yaml.h"

namespace YAML
{

template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::Variable>
{
public:
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
class convert<Antares::IO::Inputs::YmlOptimConfig::Constraint>
{
public:
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
class convert<Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing>
{
public:
    static bool decode(const Node& node,
                       Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.mode = node["mode"].as<std::string>("cyclic");
        return true;
    }
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::Objective>
{
public:
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
class convert<Antares::IO::Inputs::YmlOptimConfig::Model>
{
public:
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

        const auto& outOfBoundsProcessingNode = node["out-of-bounds-processing"];
        if (outOfBoundsProcessingNode && outOfBoundsProcessingNode["constraints"])
        {
            rhs.constraints_out_of_bounds_processing = as_fallback_default<
              std::vector<Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing>>(
              outOfBoundsProcessingNode["constraints"]);
        }

        return true;
    }
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::OptimConfig>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::OptimConfig& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }

        // Parse resolution-mode (optional, defaults to sequential-subproblems)
        if (node["resolution-mode"])
        {
            rhs.resolution_mode = node["resolution-mode"].as<std::string>();
        }

        // Parse models list
        rhs.models = node["models"].as<std::vector<Antares::IO::Inputs::YmlOptimConfig::Model>>();

        return true;
    }
};

} // namespace YAML
