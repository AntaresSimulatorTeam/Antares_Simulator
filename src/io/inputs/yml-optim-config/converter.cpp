/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/io/inputs/yml-optim-config/converter.h"

#include <algorithm>

#include <antares/exception/RuntimeError.hpp>

using namespace Antares::ModelerStudy;

namespace Antares::IO::Inputs::YmlOptimConfig
{

namespace
{

Modeler::Config::Location convertLocation(const std::string& locationStr)
{
    std::string locLower = locationStr;
    std::ranges::transform(locLower, locLower.begin(), ::tolower);
    if (locLower == "master")
    {
        return Modeler::Config::Location::MASTER;
    }
    if (locLower == "master_and_subproblems")
    {
        return Modeler::Config::Location::MASTER_AND_SUBPROBLEMS;
    }
    if (locLower == "subproblems")
    {
        return Modeler::Config::Location::SUBPROBLEMS;
    }
    throw Error::RuntimeError("Unknown location: " + locationStr);
}

Modeler::Config::Variable convertVariable(const Variable& ymlVar)
{
    try
    {
        return {ymlVar.id, convertLocation(ymlVar.location)};
    }
    catch (const Error::RuntimeError& e)
    {
        throw Error::RuntimeError("Error converting variable '" + ymlVar.id + "': " + e.what());
    }
}

Modeler::Config::Objective convertObjective(const Objective& ymlObj)
{
    try
    {
        return {ymlObj.id, convertLocation(ymlObj.location)};
    }
    catch (const Error::RuntimeError& e)
    {
        throw Error::RuntimeError("Error converting objective '" + ymlObj.id + "': " + e.what());
    }
}

std::vector<Modeler::Config::Variable> convertVariables(const Model& ymlModel)
{
    std::vector<Modeler::Config::Variable> variables;
    for (const auto& var: ymlModel.variables)
    {
        variables.push_back(convertVariable(var));
    }
    return variables;
}

std::vector<Modeler::Config::Objective> convertObjectives(const Model& ymlModel)
{
    std::vector<Modeler::Config::Objective> objectives;
    for (const auto& obj: ymlModel.objectives)
    {
        objectives.push_back(convertObjective(obj));
        std::vector<Modeler::Config::Objective> objectives;
    }
    return objectives;
}

Modeler::Config::Model convertModel(const Model& ymlModel)
{
    std::vector<Modeler::Config::Variable> variables = convertVariables(ymlModel);
    std::vector<Modeler::Config::Objective> objectives = convertObjectives(ymlModel);
    Modeler::Config::ModelDecomposition decomposition(variables, objectives);
    return Modeler::Config::Model(ymlModel.id, decomposition);
}

} // namespace

// gp : copied from readSystem.cpp, should be moved to a common utils file
static std::pair<std::string, std::string> splitLibraryModelString(const std::string& s)
{
    size_t pos = s.find('.');
    if (pos == std::string::npos)
    {
        throw std::runtime_error(s);
    }

    std::string library = s.substr(0, pos);
    std::string model = s.substr(pos + 1);
    return {library, model};
}

static SystemModel::Model& getModel(const std::vector<SystemModel::Library>& libraries,
                                    const std::string& libraryId,
                                    const std::string& modelId)
{
    auto lib = std::ranges::find_if(libraries,
                                    [&libraryId](const auto& l) { return l.Id() == libraryId; });
    if (lib == libraries.end())
    {
        throw std::runtime_error("No library found with this name: " + libraryId);
    }

    auto search = lib->Models().find(modelId);
    if (search == lib->Models().end())
    {
        throw std::runtime_error("No library found with this name: " + modelId);
    }

    return search->second;
}

SystemModel::Model& findSystemModel(const YmlOptimConfig::Model& ymlModel,
                                    const std::vector<SystemModel::Library>& libraries)
{
    const auto [libraryId, modelId] = splitLibraryModelString(ymlModel.id);
    return getModel(libraries, libraryId, modelId);
}

SystemModel::Variable& findSystemVariable(std::string variable_id, SystemModel::Model& sysModel)
{
    auto filter = [&variable_id](const SystemModel::Variable& v) { return v.Id() == variable_id; };
    auto& sysVariables = sysModel.Variables();
    auto sysVar = std::ranges::find_if(sysVariables, filter);
    if (sysVar == sysVariables.end())
    {
        throw std::runtime_error("No variable found with this name: " + variable_id);
    }
    return *sysVar;
}

void updateSystemModel(SystemModel::Model& sysModel, const YmlOptimConfig::Model& ymlModel)
{
    for (const auto& var: ymlModel.variables)
    {
        auto& sysVariable = findSystemVariable(var.id, sysModel);
        sysVariable.setLocation(convertLocation(var.location));
    }
}

void OptimConfigConverter::updateLibrairies(const OptimConfig& ymlOptimConfig,
                                            std::vector<SystemModel::Library>& libraries)
{
    for (const auto& ymlModel: ymlOptimConfig)
    {
        auto& sysModel = findSystemModel(ymlModel, libraries);
        updateSystemModel(sysModel, ymlModel);
    }

    // Same with objectives
}

} // namespace Antares::IO::Inputs::YmlOptimConfig
