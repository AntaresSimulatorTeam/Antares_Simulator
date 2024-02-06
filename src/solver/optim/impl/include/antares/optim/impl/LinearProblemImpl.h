/*
** Copyright 2007-2024 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include "../../../../../api/include/antares/optim/api/LinearProblem.h"
#include "ortools/linear_solver/linear_solver.h"

using namespace std;
using namespace Antares::optim::api;
class LinearProblemImpl : public LinearProblem
{
protected:
    MPSolver* mpSolver{};
    // This shall be removed when legacy support is abandoned
    LinearProblemImpl();
public :
    LinearProblemImpl(bool isMip, const std::string& solverName);
    MPVariable* addNumVariable(string name, double lb, double ub) override;
    MPVariable* addIntVariable(string name, double lb, double ub) override;
    MPVariable* getVariable(string name) override;
    MPConstraint* addConstraint(string name, double lb, double ub) override;
    MPConstraint* addBalanceConstraint(string name, double lb, double ub, string nodeName, int timestep) override;
    MPConstraint* getConstraint(string name) override;
    void setObjectiveCoefficient(MPVariable* variable, double coefficient) override;
    void setMinimization(bool isMinim) override;
    MipSolution solve() override;
};