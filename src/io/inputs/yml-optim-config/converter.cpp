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

#include <antares/exception/RuntimeError.hpp>

namespace Antares::IO::Inputs::YmlOptimConfig
{

namespace
{

Modeler::Config::Location convertLocation(const std::string& locationStr)
{
    return Data::Enum::fromString<Modeler::Config::Location>(locationStr);
}

Modeler::Config::Variable convertVariable(const Variable& ymlVar)
{
    return {ymlVar.id, convertLocation(ymlVar.location)};
}

Modeler::Config::Objective convertObjective(const Objective& ymlObj)
{
    return {ymlObj.id, convertLocation(ymlObj.location)};
}

Modeler::Config::Model convertModel(const Model& ymlModel)
{
    std::vector<Modeler::Config::Variable> variables;
    for (const auto& var: ymlModel.variables)
    {
        variables.push_back(convertVariable(var));
    }

    std::vector<Modeler::Config::Objective> objectives;
    for (const auto& obj: ymlModel.objectives)
    {
        objectives.push_back(convertObjective(obj));
    }

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
    return Modeler::Config::OptimConfig(models);
}

} // namespace Antares::IO::Inputs::YmlOptimConfig
