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
#pragma once

#include <ostream>
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

} // namespace Antares::Modeler::Config

namespace Antares::Modeler::Config
{

class Variable
{
public:
    Variable(std::string id, Location location):
        id_(std::move(id)),
        location_(location)
    {
    }

    const std::string& id() const
    {
        return id_;
    }

    Location location() const
    {
        return location_;
    }

private:
    std::string id_;
    Location location_;
};

class Objective
{
public:
    Objective(std::string id, Location location):
        id_(std::move(id)),
        location_(location)
    {
    }

    const std::string& id() const
    {
        return id_;
    }

    Location location() const
    {
        return location_;
    }

private:
    std::string id_;
    Location location_;
};

class ModelDecomposition
{
public:
    ModelDecomposition(std::vector<Variable> variables, std::vector<Objective> objectives):
        variables_(std::move(variables)),
        objectives_(std::move(objectives))
    {
    }

    const std::vector<Variable>& variables() const
    {
        return variables_;
    }

    const std::vector<Objective>& objectives() const
    {
        return objectives_;
    }

private:
    std::vector<Variable> variables_;
    std::vector<Objective> objectives_;
};

class Model
{
public:
    explicit Model(std::string id, ModelDecomposition modelDecomposition):
        id_(std::move(id)),
        modelDecomposition_(std::move(modelDecomposition))
    {
    }

    const std::string& id() const
    {
        return id_;
    }

    const ModelDecomposition& modelDecomposition() const
    {
        return modelDecomposition_;
    }

private:
    std::string id_;
    ModelDecomposition modelDecomposition_;
};

class OptimConfig
{
public:
    OptimConfig(std::vector<Model> models):
        models_(std::move(models))
    {
        checkDuplicateModelIds();
    }

    const std::vector<Model>& models() const
    {
        return models_;
    }

private:
    void checkDuplicateModelIds() const;

    std::vector<Model> models_;
};

} // namespace Antares::Modeler::Config
