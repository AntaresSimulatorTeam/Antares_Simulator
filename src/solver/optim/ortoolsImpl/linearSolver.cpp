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

#include <antares/solver/optim/ortoolsImpl/linearSolver.h>
#include <antares/solver/optim/ortoolsImpl/mipVariable.h>
#include <antares/solver/optim/ortoolsImpl/mipConstraint.h>

#include <antares/solver/utils/ortools_utils.h>

#include <memory>

namespace Antares::Solver::Optim::OrtoolsImpl
{

OrtoolsLinearSolver::OrtoolsLinearSolver(bool isMip, const std::string& solverName)
{
    mpSolver_ = isMip ?
        MPSolver::CreateSolver((OrtoolsUtils::solverMap.at(solverName)).MIPSolverName) :
        MPSolver::CreateSolver((OrtoolsUtils::solverMap.at(solverName)).LPSolverName);
}

Api::MipVariable* OrtoolsLinearSolver::addNumVariable(double lb, double ub, const std::string& name)
{
    auto* mpVar = mpSolver_->MakeNumVar(lb, ub, name);
    auto mipVar = std::make_shared<OrtoolsMipVariable>(mpVar);

    if (!mpVar || !mipVar)
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;

    if (!variables_.try_emplace(name, mipVar).second)
        logs.error() << "Error adding variable: " << name;


    return mipVar.get();
}

Api::MipVariable* OrtoolsLinearSolver::addIntVariable(double lb, double ub, const std::string& name)
{
    auto* mpVar = mpSolver_->MakeIntVar(lb, ub, name);
    auto mipVar = std::make_shared<OrtoolsMipVariable>(mpVar);

    if (!mpVar || !mipVar)
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;

    if (!variables_.try_emplace(name, mipVar).second)
        logs.error() << "Error adding variable: " << name;

    return mipVar.get();
}

Api::MipVariable* OrtoolsLinearSolver::getVariable(const std::string& name)
{
    return variables_.at(name).get();
}

Api::MipConstraint* OrtoolsLinearSolver::addConstraint(double lb, double ub, const std::string& name)
{
    auto* mpConstraint = mpSolver_->MakeRowConstraint(lb, ub, name);
    auto mipConstraint = std::make_shared<OrtoolsMipConstraint>(mpConstraint);

    if (!constraints_.try_emplace(name, mipConstraint).second)
        logs.error() << "Error adding constraint: " << name;

    return mipConstraint.get();
}

Api::MipConstraint* OrtoolsLinearSolver::getConstraint(const std::string& name)
{
    return constraints_.at(name).get();
}

void OrtoolsLinearSolver::setObjectiveCoefficient(Api::MipVariable* var, double coefficient)
{

}

/* double OrtoolsLinearSolver::getObjectiveCoefficient(Api::MipVariable* var) */
/* { */

/* } */

void OrtoolsLinearSolver::setMinimization()
{

}

void OrtoolsLinearSolver::setMaximization()
{

}


/* Api::MipSolution* OrtoolsLinearSolver::solve() */
/* { */

/* } */

} // namespace Antares::Solver::Optim::OrtoolsImpl
