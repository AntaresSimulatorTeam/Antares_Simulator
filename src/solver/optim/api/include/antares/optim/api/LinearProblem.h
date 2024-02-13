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

#include "ortools/linear_solver/linear_solver.h"
#include "MipSolution.h"

namespace Antares::optim::api
{
	class LinearProblem
	{
	public :
        // TODO : where to tune solver parameters?
        // TODO : should this class have native support for timestamps and scenarios?
		virtual operations_research::MPVariable& addNumVariable(std::string name, double lb, double ub) = 0;
		virtual operations_research::MPVariable& addIntVariable(std::string name, double lb, double ub) = 0;
		virtual operations_research::MPVariable& getVariable(std::string name) = 0;
		virtual operations_research::MPConstraint& addConstraint(std::string name, double lb, double ub) = 0;
        // Next method is to ensure transition with existing models. In the target solution it will be dropped and replaced by the previous one
		virtual operations_research::MPConstraint& addBalanceConstraint(std::string name, double bound, std::string nodeName, int timestep) = 0;
		virtual operations_research::MPConstraint& getConstraint(std::string name) = 0;
		virtual void setObjectiveCoefficient(const operations_research::MPVariable& variable, double coefficient) = 0;
		virtual void setMinimization(bool isMinim) = 0;
		virtual MipSolution solve() = 0;
		virtual ~LinearProblem() = default;
	};
}

