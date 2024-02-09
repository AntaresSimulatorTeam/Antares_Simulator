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

#include <vector>

#include "optim/impl/include/antares/optim/impl/LinearProblemImpl.h"
#include "utils/include/antares/solver/utils/named_problem.h"

using namespace Antares::optim::api;
using namespace std;
class LegacyLinearProblemImpl : public LinearProblemImpl
{
private:
    // For each node name, contains the balance constraint declared by the legacy filler, as well as its names & aliases declared by new fillers
    map<string, pair<MPConstraint*, vector<string>>> balanceConstraintPerNodeName;
public :
    LegacyLinearProblemImpl(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* legacyProblem,
                                     const std::string& solverName);
    MPSolver& getMpSolver() { return *mpSolver; } // only used in legacy filler
    void declareBalanceConstraint(const string& nodeName, MPConstraint* constraint); // only used in legacy filler
    MPConstraint& addBalanceConstraint(string name, double lb, string nodeName, int timestep) override;
    void setMinimization(bool isMinim) override;
};