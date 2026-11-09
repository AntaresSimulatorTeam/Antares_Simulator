// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/decoders.h"

#include <antares/io/inputs/InputError.h>

namespace YAML
{

// Keep field reads in sync with the validation schema in parser.cpp.
bool convert<Antares::IO::Inputs::YmlOptimConfig::Variable>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::Variable& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.location = node["location"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::Constraint>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::Constraint& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.location = node["location"].as<std::string>();
    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.mode = node["mode"].as<std::string>("cyclic");
    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::Objective>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::Objective& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.location = node["location"].as<std::string>();

    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::Model>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::Model& rhs)
{
    rhs.id = node["id"].as<std::string>();
    const auto& modelDecompositionNode = node["model-decomposition"];
    rhs.variables = as_fallback_default<std::vector<Antares::IO::Inputs::YmlOptimConfig::Variable>>(
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

bool convert<Antares::IO::Inputs::YmlOptimConfig::ScenarioScope>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::ScenarioScope& rhs)
{
    const auto& includeNode = node["include"];
    if (includeNode.IsDefined() && !includeNode.IsNull())
    {
        if (!includeNode.IsSequence())
        {
            throw Antares::IO::Inputs::InputError(
              "Expected a YAML sequence for 'scenario-scope.include'");
        }
        for (const auto& entry: includeNode)
        {
            if (!entry.IsScalar())
            {
                throw Antares::IO::Inputs::InputError(
                  "Expected a scalar in 'scenario-scope.include'");
            }
            rhs.include.push_back(entry.as<std::string>());
        }
    }

    const auto& excludeNode = node["exclude"];
    if (excludeNode.IsDefined() && !excludeNode.IsNull())
    {
        if (!excludeNode.IsSequence())
        {
            throw Antares::IO::Inputs::InputError(
              "Expected a YAML sequence for 'scenario-scope.exclude'");
        }
        for (const auto& entry: excludeNode)
        {
            if (!entry.IsScalar())
            {
                throw Antares::IO::Inputs::InputError(
                  "Expected a scalar in 'scenario-scope.exclude'");
            }
            rhs.exclude.push_back(entry.as<std::string>());
        }
    }

    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::OptimConfig>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::OptimConfig& rhs)
{
    // Parse resolution-mode (optional, defaults to sequential-subproblems)
    if (node["resolution-mode"])
    {
        rhs.resolution_mode = node["resolution-mode"].as<std::string>();
    }

    // Parse models list
    if (node["models"].IsDefined() && !node["models"].IsNull())
    {
        rhs.models = node["models"].as<std::vector<Antares::IO::Inputs::YmlOptimConfig::Model>>();
    }

    // Parse scenario-scope (optional, absent -> default scenario scope)
    if (node["scenario-scope"].IsDefined() && !node["scenario-scope"].IsNull())
    {
        rhs.scenario_scope = node["scenario-scope"]
                               .as<Antares::IO::Inputs::YmlOptimConfig::ScenarioScope>();
    }

    return true;
}

} // namespace YAML
