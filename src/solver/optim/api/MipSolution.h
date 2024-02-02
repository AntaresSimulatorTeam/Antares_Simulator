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
using namespace operations_research;
using namespace std;
namespace Antares::optim::api
{
    class MipSolution
    {
        // TODO : improve this
    private:
        operations_research::MPSolver::ResultStatus responseStatus_;
        map<MPVariable*, double> solution_;
    public:
        MipSolution(operations_research::MPSolver::ResultStatus responseStatus, map<MPVariable*, double> solution) : responseStatus_(responseStatus), solution_(solution) {};
        operations_research::MPSolver::ResultStatus getStatus() { return responseStatus_; }
        double getOptimalValue(MPVariable* variable) { return solution_.at(variable); }
    };
}
