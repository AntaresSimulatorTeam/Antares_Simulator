/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#pragma once

#include <map>
#include <string>

#include "mipConstraint.h"
#include "mipSolution.h"
#include "mipVariable.h"

namespace Antares::Solver::Optim::Api
{

class LinearProblem
{
public:
    virtual MipVariable* addNumVariable(double lb, double ub, const std::string& name) = 0;
    virtual MipVariable* addIntVariable(double lb, double ub, const std::string& name) = 0;
    virtual MipVariable* addVariable(double lb, double ub, bool integer, const std::string& name)
      = 0;
    virtual MipVariable* getVariable(const std::string& name) = 0;

    virtual MipConstraint* addConstraint(double lb, double ub, const std::string& name) = 0;
    virtual MipConstraint* getConstraint(const std::string& name) = 0;

    virtual void setObjectiveCoefficient(MipVariable* var, double coefficient) = 0;

    virtual double getObjectiveCoefficient(MipVariable* var) = 0;

    virtual void setMinimization() = 0;
    virtual void setMaximization() = 0;

    virtual bool isMinimization() = 0;
    virtual bool isMaximization() = 0;

    virtual MipSolution* solve(bool verboseSolver) = 0;
};

} // namespace Antares::Solver::Optim::Api
