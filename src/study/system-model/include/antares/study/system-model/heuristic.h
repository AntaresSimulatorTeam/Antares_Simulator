// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

namespace Antares::ModelerStudy::SystemModel
{

/**
 * Hard-coded thermal heuristics that can be applied, component by component, between two
 * linear optimisations (same algorithms as the legacy "fast" and "accurate" unit-commitment
 * modes).
 */
enum class HeuristicType
{
    Fast,
    Accurate
};

/**
 * Hard-coded inputs/outputs of the heuristics. optim-config.yml maps each of them to a parameter
 * or a variable of the GEMS model.
 */
enum class HeuristicElement
{
    // accurate
    NumUnitsOnOpt,
    NumUnitsMax,
    MinimumNumUnitsOn,
    // fast
    GenerationPower,
    ClusterMaxGeneration,
    MinPowerPerUnit,
    MaxPowerPerUnit,
    MinimumGenerationPower,
    // both
    MinUpDuration,
    MinDownDuration
};

enum class HeuristicInputSource
{
    Parameter,
    VariableSolution,
    VariableLowerBound,
    VariableUpperBound
};

enum class HeuristicOutputTarget
{
    VariableLowerBound,
    VariableUpperBound
};

/// Time dependency expected by the heuristic for a given element
enum class HeuristicElementTimeDependency
{
    Constant,
    TimeDependent,
    Any
};

struct HeuristicInput
{
    HeuristicElement element;
    std::string id; // parameter or variable id in the model
    HeuristicInputSource source = HeuristicInputSource::Parameter;
};

struct HeuristicOutput
{
    HeuristicElement element;
    std::string id; // variable id in the model
    HeuristicOutputTarget target;
};

struct HeuristicDefinition
{
    HeuristicType type;
    std::vector<HeuristicInput> inputs;
    std::vector<HeuristicOutput> outputs;

    const HeuristicInput* input(HeuristicElement element) const
    {
        auto it = std::ranges::find(inputs, element, &HeuristicInput::element);
        return it == inputs.end() ? nullptr : &*it;
    }

    const HeuristicOutput* output(HeuristicElement element) const
    {
        auto it = std::ranges::find(outputs, element, &HeuristicOutput::element);
        return it == outputs.end() ? nullptr : &*it;
    }
};

inline std::optional<HeuristicType> heuristicTypeFromString(const std::string& value)
{
    if (value == "fast")
    {
        return HeuristicType::Fast;
    }
    if (value == "accurate")
    {
        return HeuristicType::Accurate;
    }
    return std::nullopt;
}

inline std::string toString(HeuristicType type)
{
    return type == HeuristicType::Fast ? "fast" : "accurate";
}

inline std::string toString(HeuristicElement element)
{
    switch (element)
    {
    case HeuristicElement::NumUnitsOnOpt:
        return "num_units_on_opt";
    case HeuristicElement::NumUnitsMax:
        return "num_units_max";
    case HeuristicElement::MinimumNumUnitsOn:
        return "minimum_num_units_on";
    case HeuristicElement::GenerationPower:
        return "generation_power";
    case HeuristicElement::ClusterMaxGeneration:
        return "cluster_max_generation";
    case HeuristicElement::MinPowerPerUnit:
        return "min_power_per_unit";
    case HeuristicElement::MaxPowerPerUnit:
        return "max_power_per_unit";
    case HeuristicElement::MinimumGenerationPower:
        return "minimum_generation_power";
    case HeuristicElement::MinUpDuration:
        return "min_up_duration";
    case HeuristicElement::MinDownDuration:
        return "min_down_duration";
    default:
        return "unknown";
    }
}

/// Mandatory inputs of each heuristic (fixed schema)
inline std::vector<HeuristicElement> mandatoryInputs(HeuristicType type)
{
    using enum HeuristicElement;
    if (type == HeuristicType::Accurate)
    {
        return {NumUnitsOnOpt, NumUnitsMax, MinUpDuration, MinDownDuration};
    }
    return {GenerationPower,
            ClusterMaxGeneration,
            MinPowerPerUnit,
            MaxPowerPerUnit,
            MinUpDuration,
            MinDownDuration};
}

/// Mandatory outputs of each heuristic (fixed schema)
inline std::vector<HeuristicElement> mandatoryOutputs(HeuristicType type)
{
    if (type == HeuristicType::Accurate)
    {
        return {HeuristicElement::MinimumNumUnitsOn};
    }
    return {HeuristicElement::MinimumGenerationPower};
}

inline HeuristicElementTimeDependency expectedTimeDependency(HeuristicElement element)
{
    using enum HeuristicElement;
    switch (element)
    {
    case MinUpDuration:
    case MinDownDuration:
    case MinPowerPerUnit:
    case MaxPowerPerUnit:
        return HeuristicElementTimeDependency::Constant;
    case NumUnitsOnOpt:
    case GenerationPower:
    case MinimumNumUnitsOn:
    case MinimumGenerationPower:
        return HeuristicElementTimeDependency::TimeDependent;
    case NumUnitsMax:
    case ClusterMaxGeneration:
    default:
        return HeuristicElementTimeDependency::Any;
    }
}

} // namespace Antares::ModelerStudy::SystemModel
