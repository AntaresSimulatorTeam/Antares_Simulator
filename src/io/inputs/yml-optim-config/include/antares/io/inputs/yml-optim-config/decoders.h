
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
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Variable& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::Constraint>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Constraint& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing>
{
public:
    static bool decode(const Node& node,
                       Antares::IO::Inputs::YmlOptimConfig::ConstraintOutOfBoundsProcessing& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::Objective>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Objective& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::Model>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::Model& rhs);
};

template<>
class convert<Antares::IO::Inputs::YmlOptimConfig::OptimConfig>
{
public:
    static bool decode(const Node& node, Antares::IO::Inputs::YmlOptimConfig::OptimConfig& rhs);
};

} // namespace YAML
