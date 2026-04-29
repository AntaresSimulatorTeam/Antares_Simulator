
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

struct Model
{
    std::string id;

    std::vector<Variable> variables;
    std::vector<Constraint> constraints;
    std::vector<Objective> objectives;
    std::vector<ConstraintOutOfBoundsProcessing> constraints_out_of_bounds_processing;
};

struct OptimConfig
{
    // Resolution mode requested in the YAML file. Default value: sequential-subproblems
    std::string resolution_mode = "sequential-subproblems";

    // List of models defined in the optim-config.yaml file
    std::vector<Model> models;
};

} // namespace Antares::IO::Inputs::YmlOptimConfig
