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

#include "constraint.h"
#include "expression.h"
#include "parameter.h"
#include "variable.h"

namespace Antares::Solver::Model
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

    void getAllConstraints();

private:
    std::string id;
    Expression objective;

    std::map<std::string, Parameter> parameters_;
    std::map<std::string, Variable> variables_;

    std::map<std::string, Constraint> constraints_;
    std::map<std::string, Constraint> bindingConstraints_;
};

} // namespace Antares::Solver::Model
