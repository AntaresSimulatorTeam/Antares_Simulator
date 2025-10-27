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

Modeler::Config::OptimConfig OptimConfigConverter::convert(const OptimConfig& ymlOptimConfig)
{
    std::vector<Modeler::Config::Model> models;
    for (const auto& ymlModel: ymlOptimConfig)
    {
        models.push_back(convertModel(ymlModel));
    }
    return {models};
}

} // namespace Antares::IO::Inputs::YmlOptimConfig
