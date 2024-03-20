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
#include "LegacyLinearProblemFillerImpl.h"
#include "LegacyLinearProblemImpl.h"
#include "utils/include/antares/solver/utils/ortools_utils.h"

using namespace Antares::optim::api;

void LegacyLinearProblemFillerImpl::addVariables(LinearProblem& problem,
                                                 const LinearProblemData::YearView& data)
{
    // For now, only one "fill" method is developed in order for the legacy problem to fill the
    // ORTools matrix It is called here since addVariables is called first in problem builder If
    // needed, we shall split it up in three functions Also, no "fill(linearProblem)" has been
    // developed yet, so we are casting the linearProblem and fetching its underlying MPSolver
    // object in order to fill it. If needed, we should change this in the future.
    Antares::Optimization::ProblemSimplexeNommeConverter converter("mock", legacyProblem_);
    if (auto* legacyLinearProblem = dynamic_cast<LegacyLinearProblemImpl*>(&problem))
    {
        converter.Fill(legacyLinearProblem->getMpSolver());
        declareBalanceConstraints(legacyLinearProblem, data.legacy);
    }
    else
    {
        // throw
    }
};

void LegacyLinearProblemFillerImpl::addConstraints(LinearProblem& problem,
                                                   const LinearProblemData::YearView& data){
  // nothing to do: everything has been done in addVariables
};

void LegacyLinearProblemFillerImpl::addObjective(LinearProblem& problem,
                                                 const LinearProblemData::YearView& data)
{
    // nothing to do: everything has been done in addVariables
}

void LegacyLinearProblemFillerImpl::update(LinearProblem& problem,
                                           const LinearProblemData::YearView& data)
{
    // TODO
}

// TODO move to class LegacyLinearProblem? Maybe simpler if we don't need the "update" method after
// all

// Tell the LegacyLinearProblem what the balance constraints are, in order to be able to add new
// models to existing nodes
void LegacyLinearProblemFillerImpl::declareBalanceConstraints(
  LegacyLinearProblemImpl* legacyLinearProblem,
  const LinearProblemData::Legacy& legacy)
{
    const auto* solver = legacyLinearProblem->getMpSolver();
    auto* constraintMapping = legacy.constraintMapping;

    for (unsigned int timestamp = 0; timestamp < constraintMapping->size(); timestamp++)
    {
        const auto& BalanceAtT = constraintMapping->at(timestamp).NumeroDeContrainteDesBilansPays;
        for (unsigned areaIndex = 0; areaIndex < BalanceAtT.size(); areaIndex++)
        {
            int cnt = BalanceAtT[areaIndex];
            operations_research::MPConstraint* constraint = solver->constraint(cnt);
            legacyLinearProblem->declareBalanceConstraint(
              std::string(legacy.areaNames->at(areaIndex)), timestamp, constraint);
        }
    }
}
