/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <antares/solver/libObjectModel/model.h>

namespace Antares::Solver::ObjectModel
{

Model ModelBuilder::build()
{
    return model_;
}

ModelBuilder& ModelBuilder::withId(std::string_view id)
{
    model_.id_ = id;
    return *this;
}

ModelBuilder& ModelBuilder::withObjective(Expression objective)
{
    model_.objective_ = objective;
    return *this;
}

ModelBuilder& ModelBuilder::withParameters(std::vector<Parameter>& parameters)
{
    std::transform(parameters.begin(),
                   parameters.end(),
                   std::inserter(model_.parameters_, model_.parameters_.end()),
                   [](const Parameter& parameter)
                   { return std::make_pair(parameter.Name(), std::move(parameter)); });
    return *this;
}

ModelBuilder& ModelBuilder::withVariables(std::vector<Variable>& variables)
{
    std::transform(variables.begin(),
                   variables.end(),
                   std::inserter(model_.variables_, model_.variables_.end()),
                   [](const Variable& variable)
                   { return std::make_pair(variable.Name(), std::move(variable)); });
    return *this;
}

} // namespace Antares::Solver::ObjectModel
