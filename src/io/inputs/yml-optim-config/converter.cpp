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

Modeler::Config::Location convertLocation(const std::string& locationStr)
{
    std::string locLower = locationStr;
    std::ranges::transform(locLower, locLower.begin(), ::tolower);
    if (locLower == "master")
    {
        return Modeler::Config::Location::MASTER;
    }
    if (locLower == "master-and-subproblems")
    {
        return Modeler::Config::Location::MASTER_AND_SUBPROBLEMS;
    }
    if (locLower == "subproblems")
    {
        return Modeler::Config::Location::SUBPROBLEMS;
    }
    throw Error::RuntimeError("Unknown location: " + locationStr);
}

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
        throw std::runtime_error("No model found with this name: " + modelId);
    }

    return search->second;
}

SystemModel::Model& findSystemModel(const YmlOptimConfig::Model& ymlModel,
                                    const std::vector<SystemModel::Library>& libraries)
{
    const auto [libraryId, modelId] = splitLibraryModelString(ymlModel.id);
    return getModel(libraries, libraryId, modelId);
}

SystemModel::Variable& findSystemVariable(const std::string& var_id, SystemModel::Model& sysModel)
{
    auto filter = [&var_id](const SystemModel::Variable& v) { return v.Id() == var_id; };
    auto& sysVariables = sysModel.Variables();
    auto sysVar = std::ranges::find_if(sysVariables, filter);
    if (sysVar == sysVariables.end())
    {
        throw std::runtime_error("No variable found with this name: " + var_id);
    }
    return *sysVar;
}

SystemModel::Objective& findSystemObjective(const std::string& obj_id, SystemModel::Model& sysModel)
{
    auto filter = [&obj_id](const SystemModel::Objective& obj) { return obj.Id() == obj_id; };
    auto& sysObjectives = sysModel.Objectives();
    auto sysObj = std::ranges::find_if(sysObjectives, filter);
    if (sysObj == sysObjectives.end())
    {
        throw std::runtime_error("No objective found with this name: " + obj_id);
    }
    return *sysObj;
}

void updateSystemModel(SystemModel::Model& sysModel, const YmlOptimConfig::Model& ymlModel)
{
    for (const auto& ymlVar: ymlModel.variables)
    {
        auto& sysVariable = findSystemVariable(ymlVar.id, sysModel);
        sysVariable.setLocation(convertLocation(ymlVar.location));
    }

    for (const auto& ymlObj: ymlModel.objectives)
    {
        auto& sysObjective = findSystemObjective(ymlObj.id, sysModel);
        sysObjective.setLocation(convertLocation(ymlObj.location));
    }
}

void updateLibrairies(const OptimConfig& ymlOptimConfig,
                      std::vector<SystemModel::Library>& libraries)
{
    for (const auto& ymlModel: ymlOptimConfig)
    {
        auto& sysModel = findSystemModel(ymlModel, libraries);
        updateSystemModel(sysModel, ymlModel);
    }
}

} // namespace Antares::IO::Inputs::YmlOptimConfig
