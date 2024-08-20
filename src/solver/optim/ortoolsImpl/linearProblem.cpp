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

#include <exception>
#include <memory>
#include <ortools/linear_solver/linear_solver.h>

#include <antares/solver/optim/ortoolsImpl/linearProblem.h>
#include <antares/solver/utils/ortools_utils.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

OrtoolsLinearProblem::OrtoolsLinearProblem(bool isMip, const std::string& solverName)
{
    auto* mpSolver = isMip ? MPSolver::CreateSolver(
                               (OrtoolsUtils::solverMap.at(solverName)).MIPSolverName)
                           : MPSolver::CreateSolver(
                               (OrtoolsUtils::solverMap.at(solverName)).LPSolverName);

    mpSolver_ = std::unique_ptr<operations_research::MPSolver>(mpSolver);
    objective_ = mpSolver->MutableObjective();

    params_.SetIntegerParam(MPSolverParameters::SCALING, 0);
    params_.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);
}

class ElemAlreadyExists: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Element name already exists in linear problem";
    }
};

Api::MipVariable* OrtoolsLinearProblem::addVariable(double lb,
                                                    double ub,
                                                    bool integer,
                                                    const std::string& name)
{
    if (variables_.contains(name))
    {
        logs.error() << "This variable already exists: " << name;
        throw ElemAlreadyExists();
    }

    auto* mpVar = mpSolver_->MakeVar(lb, ub, integer, name);
    auto mipVar = std::make_unique<OrtoolsMipVariable>(mpVar);

    if (!mpVar || !mipVar)
    {
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;
    }

    const auto& mapIteratorPair = variables_.try_emplace(name, std::move(mipVar));

    if (!mapIteratorPair.second)
    {
        logs.error() << "Error adding variable: " << name;
    }

    return mapIteratorPair.first->second.get(); // <<name, var>, bool>
}

Api::MipVariable* OrtoolsLinearProblem::addNumVariable(double lb,
                                                       double ub,
                                                       const std::string& name)
{
    return addVariable(lb, ub, false, name);
}

Api::MipVariable* OrtoolsLinearProblem::addIntVariable(double lb,
                                                       double ub,
                                                       const std::string& name)
{
    return addVariable(lb, ub, true, name);
}

Api::MipVariable* OrtoolsLinearProblem::getVariable(const std::string& name)
{
    return variables_[name].get();
}

Api::MipConstraint* OrtoolsLinearProblem::addConstraint(double lb,
                                                        double ub,
                                                        const std::string& name)
{
    if (constraints_.contains(name))
    {
        logs.error() << "This constraint already exists: " << name;
        throw ElemAlreadyExists();
    }

    auto* mpConstraint = mpSolver_->MakeRowConstraint(lb, ub, name);
    auto mipConstraint = std::make_unique<OrtoolsMipConstraint>(mpConstraint);

    if (!mpConstraint || !mipConstraint)
    {
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;
    }

    const auto& mapIteratorPair = constraints_.try_emplace(name, std::move(mipConstraint));

    if (!mapIteratorPair.second)
    {
        logs.error() << "Error adding variable: " << name;
    }

    return mapIteratorPair.first->second.get(); // <<name, constraint>, bool>
}

Api::MipConstraint* OrtoolsLinearProblem::getConstraint(const std::string& name)
{
    return constraints_[name].get();
}

static const operations_research::MPVariable* getMpVar(const Api::MipVariable* var)

{
    const auto* orMpVar = dynamic_cast<const OrtoolsMipVariable*>(var);
    if (!orMpVar)
    {
        logs.error() << "Invalid cast, tried from Api::MipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }
    return orMpVar->getMpVar();
}

void OrtoolsLinearProblem::setObjectiveCoefficient(Api::MipVariable* var, double coefficient)
{
    objective_->SetCoefficient(getMpVar(var), coefficient);
}

double OrtoolsLinearProblem::getObjectiveCoefficient(const Api::MipVariable* var) const
{
    return objective_->GetCoefficient(getMpVar(var));
}

void OrtoolsLinearProblem::setMinimization()
{
    objective_->SetMinimization();
}

void OrtoolsLinearProblem::setMaximization()
{
    objective_->SetMaximization();
}

bool OrtoolsLinearProblem::isMinimization() const
{
    return objective_->minimization();
}

bool OrtoolsLinearProblem::isMaximization() const
{
    return objective_->maximization();
}

static Api::MipStatus convertStatus(operations_research::MPSolver::ResultStatus& status)
{
    switch (status)
    {
    case operations_research::MPSolver::ResultStatus::OPTIMAL:
        return Api::MipStatus::OPTIMAL;
    case operations_research::MPSolver::ResultStatus::FEASIBLE:
        return Api::MipStatus::FEASIBLE;
    case operations_research::MPSolver::ResultStatus::UNBOUNDED:
        return Api::MipStatus::UNBOUNDED;
    default:
        break;
    }
    return Api::MipStatus::MIP_ERROR;
}

Api::MipSolution* OrtoolsLinearProblem::solve(bool verboseSolver)
{
    if (verboseSolver)
    {
        mpSolver_->EnableOutput();
    }

    auto mpStatus = mpSolver_->Solve(params_);
    Api::MipStatus status = convertStatus(mpStatus);

    std::map<std::string, std::pair<Api::MipVariable*, double>> solution;
    for (const auto* var: mpSolver_->variables())
    {
        auto pair = std::make_pair(variables_.at(var->name()).get(), var->solution_value());
        solution.try_emplace(var->name(), pair);
    }

    solution_ = std::make_unique<OrtoolsMipSolution>(solution, status, objective_->Value());
    return solution_.get();
}

} // namespace Antares::Solver::Optim::OrtoolsImpl
