
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Antares::IO::Inputs::YmlOptimConfig
{

struct Variable
{
    std::string id;
    std::string location;
};

struct Constraint
{
    std::string id;
    std::string location;
};

struct ConstraintOutOfBoundsProcessing
{
    std::string id;
    std::string mode;
};

struct Objective
{
    std::string id;
    std::string location;
};

enum class HeuristicType
{
    Accurate,
    Fast
};

enum class HeuristicElement
{
    NumUnitsOnOpt,
    NumUnitsMax,
    MinUpDuration,
    MinDownDuration,

    GenerationPower,
    ClusterMaxGeneration,
    MinPowerPerUnit,
    MaxPowerPerUnit,

    MinimumNumUnitsOn,
    MinimumGenerationPower
};

inline bool isValidHeuristicElement(HeuristicType heuristic, HeuristicElement element)
{
    switch (heuristic)
    {
    case HeuristicType::Accurate:
        return element == HeuristicElement::NumUnitsOnOpt
               || element == HeuristicElement::NumUnitsMax
               || element == HeuristicElement::MinUpDuration
               || element == HeuristicElement::MinDownDuration
               || element == HeuristicElement::MinimumNumUnitsOn;

    case HeuristicType::Fast:
        return element == HeuristicElement::GenerationPower
               || element == HeuristicElement::ClusterMaxGeneration
               || element == HeuristicElement::MinPowerPerUnit
               || element == HeuristicElement::MaxPowerPerUnit
               || element == HeuristicElement::MinUpDuration
               || element == HeuristicElement::MinDownDuration
               || element == HeuristicElement::MinimumGenerationPower;
    }

    return false;
}

enum class HeuristicInputType
{
    VariableSolution,
    VariableUpperBound,
    VariableLowerBound,
    Parameter
};

enum class HeuristicOutputType
{
    VariableUpperBound,
    VariableLowerBound
};

struct HeuristicInput
{
    HeuristicElement heuristic_element;
    std::string id;
    HeuristicInputType type = HeuristicInputType::Parameter;
};

struct HeuristicOutput
{
    HeuristicElement heuristic_element;
    std::string id;
    HeuristicOutputType type;
};

struct Heuristic
{
    HeuristicType type;
    std::string id;

    std::vector<HeuristicInput> inputs;
    std::vector<HeuristicOutput> outputs;
};

struct Model
{
    std::string id;

    std::vector<Variable> variables;
    std::vector<Constraint> constraints;
    std::vector<Objective> objectives;
    std::vector<ConstraintOutOfBoundsProcessing> constraints_out_of_bounds_processing;
    bool isHeuristic = false;
    std::vector<Heuristic> heuristics;
};

struct ScenarioScope
{
    // Inline form: individual integers, string integers, and inclusive "a-b" range strings.
    std::vector<std::string> include;
    // Scenarios to remove from the base set (optional).
    std::vector<std::string> exclude;
};

struct OptimConfig
{
    // Resolution mode requested in the YAML file. Default value: sequential-subproblems
    std::string resolution_mode = "sequential-subproblems";

    // List of models defined in the optim-config.yaml file
    std::vector<Model> models;

    // Monte-Carlo scenarios to simulate (optional, absent -> default scenario scope)
    std::optional<ScenarioScope> scenario_scope;
};

} // namespace Antares::IO::Inputs::YmlOptimConfig
