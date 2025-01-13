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

/// Namespace for the classes related to the linear problem API
namespace Antares::Solver::Modeler::Api
{

/**
 * Linear Problem
 * This class is aimed at creating and manipulating variables/constraints
 * Also used to control the objective, maximization or minimization, and to solve the problem
 */
class ILinearProblem
{
public:
    virtual ~ILinearProblem() = default;

    /// Create a continuous variable
    virtual IMipVariable* addNumVariable(double lb, double ub, const std::string& name) = 0;
    /// Create a integer variable
    virtual IMipVariable* addIntVariable(double lb, double ub, const std::string& name) = 0;
    /// Create a continuous or integer variable
    virtual IMipVariable* addVariable(double lb, double ub, bool integer, const std::string& name)
      = 0;
    virtual IMipVariable* getVariable(const std::string& name) const = 0;
    virtual int variableCount() const = 0;

    /// Add a bounded constraint to the problem
    virtual IMipConstraint* addConstraint(double lb, double ub, const std::string& name) = 0;
    virtual IMipConstraint* getConstraint(const std::string& name) const = 0;
    virtual int constraintCount() const = 0;

    /// Set the objective coefficient for a given variable
    virtual void setObjectiveCoefficient(IMipVariable* var, double coefficient) = 0;
    virtual double getObjectiveCoefficient(const IMipVariable* var) const = 0;

    /// Sets the optimization direction to minimize
    virtual void setMinimization() = 0;
    /// Sets the optimization direction to maximize
    virtual void setMaximization() = 0;

    virtual bool isMinimization() const = 0;
    virtual bool isMaximization() const = 0;

    /// Solve the problem, returns a IMipSolution
    virtual IMipSolution* solve(bool verboseSolver) = 0;

    virtual void WriteLP(const std::string& filename) = 0;

    // Definition of infinity
    virtual double infinity() const = 0;
};

} // namespace Antares::Solver::Modeler::Api
