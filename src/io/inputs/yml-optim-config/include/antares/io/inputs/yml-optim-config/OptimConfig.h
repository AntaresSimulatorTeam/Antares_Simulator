
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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

struct Objective
{
    std::string id;
    std::string location;
};

struct Model
{
    std::string id;

    std::vector<Variable> variables;
    std::vector<Constraint> constraints;
    std::vector<Objective> objectives;
};

using OptimConfig = std::vector<Model>;

} // namespace Antares::IO::Inputs::YmlOptimConfig
