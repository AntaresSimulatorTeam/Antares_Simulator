// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-optim-config/decoders.h"

#include <antares/io/inputs/InputError.h>
using namespace Antares::IO::Inputs::YmlOptimConfig;

namespace YAML
{

// Keep field reads in sync with the validation schema in parser.cpp.

namespace
{
HeuristicType parseHeuristicType(const std::string& value)
{
    if (value == "accurate")
    {
        return HeuristicType::Accurate;
    }

    if (value == "fast")
    {
        return HeuristicType::Fast;
    }

    throw Antares::IO::Inputs::InputError("Unknown heuristic type: " + value);
}

HeuristicOutputType parseHeuristicOutputType(const std::string& value)
{
    if (value == "variable-upper-bound")
    {
        return HeuristicOutputType::VariableUpperBound;
    }

    if (value == "variable-lower-bound")
    {
        return HeuristicOutputType::VariableLowerBound;
    }

    throw Antares::IO::Inputs::InputError("Unknown heuristic output type: " + value);
}

HeuristicInputType parseHeuristicInputType(const std::string& value)
{
    if (value == "variable-solution")
    {
        return HeuristicInputType::VariableSolution;
    }

    if (value == "variable-upper-bound")
    {
        return HeuristicInputType::VariableUpperBound;
    }

    if (value == "variable-lower-bound")
    {
        return HeuristicInputType::VariableLowerBound;
    }

    if (value == "parameter")
    {
        return HeuristicInputType::Parameter;
    }

    throw Antares::IO::Inputs::InputError("Unknown heuristic input type: " + value);
}

HeuristicElement parseHeuristicElement(const std::string& value)
{
    if (value == "num_units_on_opt")
    {
        return HeuristicElement::NumUnitsOnOpt;
    }

    if (value == "num_units_max")
    {
        return HeuristicElement::NumUnitsMax;
    }

    if (value == "min_up_duration")
    {
        return HeuristicElement::MinUpDuration;
    }

    if (value == "min_down_duration")
    {
        return HeuristicElement::MinDownDuration;
    }

    if (value == "generation_power")
    {
        return HeuristicElement::GenerationPower;
    }

    if (value == "cluster_max_generation")
    {
        return HeuristicElement::ClusterMaxGeneration;
    }

    if (value == "min_power_per_unit")
    {
        return HeuristicElement::MinPowerPerUnit;
    }

    if (value == "max_power_per_unit")
    {
        return HeuristicElement::MaxPowerPerUnit;
    }

    if (value == "minimum_num_units_on")
    {
        return HeuristicElement::MinimumNumUnitsOn;
    }

    if (value == "minimum_generation_power")
    {
        return HeuristicElement::MinimumGenerationPower;
    }

    throw Antares::IO::Inputs::InputError("Unknown heuristic element: " + value);
}

} // namespace

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

bool convert<Antares::IO::Inputs::YmlOptimConfig::HeuristicInput>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::HeuristicInput& rhs)
{
    rhs.heuristic_element = parseHeuristicElement(node["heuristic-element"].as<std::string>());

    // The specification uses both "id" and "value" to name the model parameter/variable
    if (node["id"])
    {
        rhs.id = node["id"].as<std::string>();
    }
    else if (node["value"])
    {
        rhs.id = node["value"].as<std::string>();
    }
    else
    {
        throw Antares::IO::Inputs::InputError("Missing 'id' for heuristic input '"
                                              + node["heuristic-element"].as<std::string>() + "'");
    }

    if (node["type"])
    {
        rhs.type = parseHeuristicInputType(node["type"].as<std::string>());
    }

    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::HeuristicOutput>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::HeuristicOutput& rhs)
{
    rhs.heuristic_element = parseHeuristicElement(node["heuristic-element"].as<std::string>());

    rhs.id = node["id"].as<std::string>();

    rhs.type = parseHeuristicOutputType(node["type"].as<std::string>());

    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::Heuristic>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::Heuristic& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.type = parseHeuristicType(rhs.id);
    for (const auto& element: {"inputs", "outputs"})
    {
        if (node[element] && !node[element].IsSequence())
        {
            throw Antares::IO::Inputs::InputError(std::string("Heuristic '") + rhs.id + "': '"
                                                  + element + "' must be a list");
        }
    }

    rhs.inputs = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlOptimConfig::HeuristicInput>>(node["inputs"]);

    rhs.outputs = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlOptimConfig::HeuristicOutput>>(node["outputs"]);

    return true;
}

bool convert<Antares::IO::Inputs::YmlOptimConfig::Model>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlOptimConfig::Model& rhs)
{
    rhs.id = node["id"].as<std::string>();
    const auto& modelDecompositionNode = node["model-decomposition"];
    if (!modelDecompositionNode && !node["heuristics"])
    {
        // The schema in parser.cpp cannot express "one of": checked here
        throw Antares::IO::Inputs::InputError("Model '" + rhs.id
                                              + "': Missing field: model-decomposition (or "
                                                "heuristics)");
    }
    if (modelDecompositionNode)
    {
        rhs.variables = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::Variable>>(
          modelDecompositionNode["variables"]);

        rhs.constraints = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::Constraint>>(
          modelDecompositionNode["constraints"]);

        rhs.objectives = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::Objective>>(
          modelDecompositionNode["objective-contributions"]);
    }

    const auto& outOfBoundsProcessingNode = node["out-of-bounds-processing"];
    if (outOfBoundsProcessingNode && outOfBoundsProcessingNode["constraints"])
    {
        rhs.constraints_out_of_bounds_processing = as_fallback_default<
          std::vector<Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing>>(
          outOfBoundsProcessingNode["constraints"]);
    }

    rhs.heuristics = as_fallback_default<
      std::vector<Antares::IO::Inputs::YmlOptimConfig::Heuristic>>(node["heuristics"]);

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
