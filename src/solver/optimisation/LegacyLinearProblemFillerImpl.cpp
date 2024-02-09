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

void LegacyLinearProblemFillerImpl::addVariables(LinearProblem& problem, const LinearProblemData& data)
{
    // For now, only one "fill" method is developed in order for the legacy problem to fill the ORTools matrix
    // It is called here since addVariables is called first in problem builder
    // If needed, we shall split it up in three functions
    // Also, no "fill(linearProblem)" has been developed yet, so we are casting the linearProblem and fetching its
    // underlying MPSolver object in order to fill it. If needed, we should change this in the future.
    Antares::Optimization::ProblemSimplexeNommeConverter converter("mock", legacyProblem_);
    if (auto *legacyLinearProblem = dynamic_cast<LegacyLinearProblemImpl *>(&problem)) {
        converter.Fill(&legacyLinearProblem->getMpSolver());
        declareBalanceConstraints(legacyLinearProblem, data);
    } else {
        // throw
    }
};

void LegacyLinearProblemFillerImpl::addConstraints(LinearProblem& problem, const LinearProblemData& data)
{
// nothing to do: everything has been done in addVariables
};

void LegacyLinearProblemFillerImpl::addObjective(LinearProblem& problem, const LinearProblemData& data)
{
// nothing to do: everything has been done in addVariables
}

void LegacyLinearProblemFillerImpl::update(LinearProblem& problem, const LinearProblemData& data)
{
// TODO
}

// TODO move to class LegacyLinearProblem ?

// Tell the LegacyLinearProblem what the balance constraints are, in order to be able to add new models to existing nodes
// TODO : ask for help for this
// utiliser data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesBilansPays[pays]
// renvoie le numéro de contrainte qui devrait marcher avec MPSolver
// MPSolver.constraint(int) renvoie une MPConstraint
void LegacyLinearProblemFillerImpl::declareBalanceConstraints(LegacyLinearProblemImpl *legacyLinearProblem,
                                                              const LinearProblemData& data)
{
   auto& balanceConstraintPerNodeName = legacyLinearProblem->balanceConstraintPerNodeName;
   const auto& solver = legacyLinearProblem->getMpSolver();
   auto* legacyCntMapping = data.legacy.CntMapping;

   for (unsigned int timestep = 0; timestep < legacyCntMapping->size(); timestep++)
   {
       const auto& BalanceAtT = legacyCntMapping->at(timestep).NumeroDeContrainteDesBilansPays;
       for (unsigned areaIndex = 0; areaIndex < BalanceAtT.size(); areaIndex++)
       {
           std::string nodeWithTs = std::string(data.legacy.areaNames->at(areaIndex)) + "_" + to_string(timestep);
           std::string name = "AreaBalance";
           int cnt = BalanceAtT[areaIndex];
           // add new name declared by filler to list of aliases of the existing constraint
           operations_research::MPConstraint* constraint = solver.constraint(cnt);
           balanceConstraintPerNodeName.insert({nodeWithTs, {constraint, {name}}});
       }
   }
}



