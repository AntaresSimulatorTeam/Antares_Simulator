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
#include <fstream>
#include <memory>
#include <ortools/linear_solver/linear_solver.h>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/ortoolsImpl/linearProblem.h>
#include <antares/solver/utils/ortools_utils.h>

namespace Antares::Solver::Modeler::OrtoolsImpl
{

OrtoolsLinearProblem::OrtoolsLinearProblem(bool isMip, const std::string& solverName)
{
    mpSolver_ = MPSolverFactory(isMip, solverName);
    objective_ = mpSolver_->MutableObjective();
}

class ElemAlreadyExists: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Element name already exists in linear problem";
    }
};

OrtoolsMipVariable* OrtoolsLinearProblem::addVariable(double lb,
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

    if (!mpVar)
    {
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;
    }

    const auto& pair = variables_.emplace(name, std::make_unique<OrtoolsMipVariable>(mpVar));
    return pair.first->second.get(); // <<name, var>, bool>
}

std::vector<Api::IMipVariable*> OrtoolsLinearProblem::addVariable(double lb,
                                                                  double ub,
                                                                  bool integer,
                                                                  const std::string& name,
                                                                  unsigned int number_new_variables)
{
    std::vector<Api::IMipVariable*> new_variables;
    for (unsigned int i = 0; i < number_new_variables; i++)
    {
        new_variables.push_back(addVariable(lb, ub, integer, name + '_' + std::to_string(i)));
    }
    return new_variables;
}

OrtoolsMipVariable* OrtoolsLinearProblem::addNumVariable(double lb,
                                                         double ub,
                                                         const std::string& name)
{
    return addVariable(lb, ub, false, name);
}

std::vector<Api::IMipVariable*> OrtoolsLinearProblem::addNumVariable(
  double lb,
  double ub,
  const std::string& name,
  unsigned int number_new_variables)
{
    std::vector<Api::IMipVariable*> new_variables;
    for (unsigned int i = 0; i < number_new_variables; i++)
    {
        new_variables.push_back(addNumVariable(lb, ub, name + '_' + std::to_string(i)));
    }
    return new_variables;
}

OrtoolsMipVariable* OrtoolsLinearProblem::addIntVariable(double lb,
                                                         double ub,
                                                         const std::string& name)
{
    return addVariable(lb, ub, true, name);
}

std::vector<Api::IMipVariable*> OrtoolsLinearProblem::addIntVariable(
  double lb,
  double ub,
  const std::string& name,
  unsigned int number_new_variables)
{
    std::vector<Api::IMipVariable*> new_variables;
    for (unsigned int i = 0; i < number_new_variables; i++)
    {
        new_variables.push_back(addIntVariable(lb, ub, name + '_' + std::to_string(i)));
    }
    return new_variables;
}

OrtoolsMipVariable* OrtoolsLinearProblem::getVariable(const std::string& name) const
{
    return variables_.at(name).get();
}

int OrtoolsLinearProblem::variableCount() const
{
    return mpSolver_->NumVariables();
}

OrtoolsMipConstraint* OrtoolsLinearProblem::addConstraint(double lb,
                                                          double ub,
                                                          const std::string& name)
{
    if (constraints_.contains(name))
    {
        logs.error() << "This constraint already exists: " << name;
        throw ElemAlreadyExists();
    }

    auto* mpConstraint = mpSolver_->MakeRowConstraint(lb, ub, name);

    if (!mpConstraint)
    {
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;
    }

    const auto& pair = constraints_.emplace(name,
                                            std::make_unique<OrtoolsMipConstraint>(mpConstraint));
    return pair.first->second.get(); // <<name, constraint>, bool>
}

std::vector<Api::IMipConstraint*> OrtoolsLinearProblem::addConstraint(
  double lb,
  double ub,
  const std::string& name,
  unsigned int number_new_constraints)
{
    std::vector<Api::IMipConstraint*> new_constraints;
    for (unsigned int i = 0; i < number_new_constraints; i++)
    {
        new_constraints.push_back(addConstraint(lb, ub, name + '_' + std::to_string(i)));
    }
    return new_constraints;
}

OrtoolsMipConstraint* OrtoolsLinearProblem::getConstraint(const std::string& name) const
{
    return constraints_.at(name).get();
}

int OrtoolsLinearProblem::constraintCount() const
{
    return mpSolver_->NumConstraints();
}

static const operations_research::MPVariable* getMpVar(const Api::IMipVariable* var)

{
    const auto* OrtoolsMipVar = dynamic_cast<const OrtoolsMipVariable*>(var);
    if (!OrtoolsMipVar)
    {
        logs.error() << "Invalid cast, tried from Api::IMipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }
    return OrtoolsMipVar->getMpVar();
}

void OrtoolsLinearProblem::setObjectiveCoefficient(Api::IMipVariable* var, double coefficient)
{
    objective_->SetCoefficient(getMpVar(var), coefficient);
}

double OrtoolsLinearProblem::getObjectiveCoefficient(const Api::IMipVariable* var) const
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

void OrtoolsLinearProblem::WriteLP(const std::string& filename)
{
    std::string out;
    mpSolver_->ExportModelAsLpFormat(false, &out);
    std::ofstream of(filename);
    of << out;
}

MPSolver* OrtoolsLinearProblem::MpSolver() const
{
    return mpSolver_;
}

OrtoolsMipSolution* OrtoolsLinearProblem::solve(bool verboseSolver)
{
    if (verboseSolver)
    {
        mpSolver_->EnableOutput();
    }

    auto mpStatus = mpSolver_->Solve(params_);

    solution_ = std::make_unique<OrtoolsMipSolution>(mpStatus, mpSolver_);
    return solution_.get();
}

double OrtoolsLinearProblem::infinity() const
{
    return MPSolver::infinity();
}

} // namespace Antares::Solver::Modeler::OrtoolsImpl
