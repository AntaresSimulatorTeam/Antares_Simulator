/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <string>
#include <vector>

namespace Antares::Modeler::Config
{

enum class Location
{
    MASTER,
    MASTER_AND_SUBPROBLEMS,
    SUBPROBLEMS
};

class Variable
{
public:
    Variable(std::string id, Location location):
        id(std::move(id)),
        location(location)
    {
    }

private:
    std::string id;
    Location location;
};

class Objective
{
public:
    Objective(std::string id, Location location):
        id(std::move(id)),
        location(location)
    {
    }

private:
    std::string id;
    Location location;
};

class ModelDecomposition
{
public:
    ModelDecomposition(std::vector<Variable> variables, std::vector<Objective> objectives):
        variables(std::move(variables)),
        objectives(std::move(objectives))
    {
    }

private:
    std::vector<Variable> variables;
    std::vector<Objective> objectives;
};

class Model
{
public:
    Model(std::string id, ModelDecomposition modelDecomposition):
        id(std::move(id)),
        modelDecomposition(std::move(modelDecomposition))
    {
    }

private:
    std::string id;
    ModelDecomposition modelDecomposition;
};

class OptimConfig
{
private:
    std::vector<std::string> modelLibraries;
    std::vector<Model> models;

    friend class OptimConfigBuilder;
};

class OptimConfigBuilder
{
public:
    OptimConfigBuilder& withId(const std::string& id);
    OptimConfigBuilder& withVariables(std::vector<Variable>&& variables);
    OptimConfigBuilder& withObjectives(std::vector<Objective>&& objectives);
    OptimConfig build();

private:
    OptimConfig config_;
};

} // namespace Antares::Modeler::Config
