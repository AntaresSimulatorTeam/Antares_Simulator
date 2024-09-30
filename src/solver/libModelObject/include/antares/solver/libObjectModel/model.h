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
#pragma once

#include <map>
#include <vector>

#include "constraint.h"
#include "expression.h"
#include "parameter.h"
#include "port.h"
#include "variable.h"

namespace Antares::Solver::ObjectModel
{

/**
 * Defines a model that can be referenced by actual components.
 * A model defines the behaviour of those components.
 */
class Model
{
public:
    Model();
    ~Model() = default;

    std::vector<Constraint*> getConstraints();

private:
    std::string id_;
    Expression objective_;

    std::map<std::string, Parameter> parameters_;
    std::map<std::string, Variable> variables_;

    std::map<std::string, Constraint> constraints_;
    std::map<std::string, Constraint> bindingConstraints_;

    std::map<std::string, Port> ports_;
};

} // namespace Antares::Solver::ObjectModel
