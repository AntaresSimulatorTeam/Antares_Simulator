/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/solver/utils/ortools_utils.h>
#include "antares/optimisation/linear-problem-mpsolver-impl/mipVariable.h"

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

void Write(const OrtoolsLinearProblem& problem, const std::filesystem::path& path)
{
    std::string out;
    problem.mpSolver_->ExportModelAsMpsFormat(/* fixed_format (ignored) */ false,
                                              /* obfuscate */ false,
                                              &out);
    std::ofstream of(path);
    of << out;
}

// Sirius solver to store objective offset
class SiriusObjectiveOffsetHandler: public ObjectiveOffsetHandler
{
public:
    void setOffset(double offset) override
    {
        offset_ = offset;
    }

    [[nodiscard]] double getOffset() const override
    {
        return offset_;
    }

    ~SiriusObjectiveOffsetHandler() override = default;

private:
    double offset_{0.0};
};

// Other solvers than Sirius (Xpress, etc.) know how to handle objective offset
class GenericOffsetHandler: public ObjectiveOffsetHandler
{
public:
    explicit GenericOffsetHandler(operations_research::MPObjective* objective):
        objective_(objective)
    {
    }

    void setOffset(double offset) override
    {
        objective_->SetOffset(offset);
    }

    [[nodiscard]] double getOffset() const override
    {
        return objective_->offset();
    }

    ~GenericOffsetHandler() override = default;

private:
    operations_research::MPObjective* objective_{nullptr};
};

std::unique_ptr<ObjectiveOffsetHandler> offsetHandlerFactory(
  const std::string& solverName,
  operations_research::MPObjective* objective)
{
    if (solverName == "sirius")
    {
        return std::make_unique<SiriusObjectiveOffsetHandler>();
    }
    else
    {
        return std::make_unique<GenericOffsetHandler>(objective);
    }
}

OrtoolsLinearProblem::OrtoolsLinearProblem(bool isMip, const std::string& solverName):
    mpSolver_(MPSolverFactory(isMip, solverName)),
    objective_(mpSolver_->MutableObjective()),
    isLP_(!isMip),
    offsetHandler_(offsetHandlerFactory(solverName, objective_))
{
}

LinearProblemApi::IMipVariable* OrtoolsLinearProblem::addVariable(double lb,
                                                                  double ub,
                                                                  bool integer,
                                                                  const std::string& name)
{
    if (ub < lb)
    {
        std::ostringstream os;
        os << "Variable '" << name << "' has invalid bounds [" << lb << ", " << ub << "] (ub < lb)";
        throw std::invalid_argument(os.str());
    }
    auto* mpVar = mpSolver_->MakeVar(lb, ub, integer, name);
    isLP_ = isLP_ && !integer;
    if (!mpVar)
    {
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;
    }

    variables_.push_back(std::make_unique<OrtoolsMipVariable>(mpVar));
    return variables_.back().get();
}

const std::vector<std::unique_ptr<LinearProblemApi::IMipVariable>>&
OrtoolsLinearProblem::getVariables() const
{
    return variables_;
}

LinearProblemApi::IMipVariable* OrtoolsLinearProblem::addNumVariable(double lb,
                                                                     double ub,
                                                                     const std::string& name)
{
    return addVariable(lb, ub, false, name);
}

LinearProblemApi::IMipVariable* OrtoolsLinearProblem::addIntVariable(double lb,
                                                                     double ub,
                                                                     const std::string& name)
{
    return addVariable(lb, ub, true, name);
}

LinearProblemApi::IMipVariable* OrtoolsLinearProblem::getVariable(std::size_t index) const
{
    return variables_.at(index).get();
}

LinearProblemApi::IMipVariable* OrtoolsLinearProblem::lookupVariable(const std::string& name) const
{
    auto it = std::ranges::find_if(variables_,
                                   [&name](const std::unique_ptr<LinearProblemApi::IMipVariable>& v)
                                   { return v->getName() == name; });
    if (it != variables_.end())
    {
        return it->get();
    }
    return nullptr;
}

LinearProblemApi::IMipConstraint* OrtoolsLinearProblem::lookupConstraint(
  const std::string& name) const
{
    auto it = std::ranges::find_if(constraints_,
                                   [&name](
                                     const std::unique_ptr<LinearProblemApi::IMipConstraint>& c)
                                   { return c->getName() == name; });
    if (it != constraints_.end())
    {
        return it->get();
    }
    return nullptr;
}

int OrtoolsLinearProblem::variableCount() const
{
    return mpSolver_->NumVariables();
}

LinearProblemApi::IMipConstraint* OrtoolsLinearProblem::addConstraint(double lb,
                                                                      double ub,
                                                                      const std::string& name)
{
    auto* mpConstraint = mpSolver_->MakeRowConstraint(lb, ub, name);

    if (!mpConstraint)
    {
        logs.error() << "Couldn't add variable to Ortools MPSolver: " << name;
    }

    constraints_.push_back(std::make_unique<OrtoolsMipConstraint>(mpConstraint));
    return constraints_.back().get();
}

const std::vector<std::unique_ptr<LinearProblemApi::IMipConstraint>>&
OrtoolsLinearProblem::getConstraints() const
{
    return constraints_;
}

LinearProblemApi::IMipConstraint* OrtoolsLinearProblem::getConstraint(std::size_t index) const
{
    return constraints_.at(index).get();
}

int OrtoolsLinearProblem::constraintCount() const
{
    return mpSolver_->NumConstraints();
}

static const operations_research::MPVariable* getMpVar(const LinearProblemApi::IMipVariable* var)

{
    const auto* OrtoolsMipVar = dynamic_cast<const OrtoolsMipVariable*>(var);
    if (!OrtoolsMipVar)
    {
        logs.error()
          << "Invalid cast, tried from LinearProblemApi::IMipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }
    return OrtoolsMipVar->getMpVar();
}

void OrtoolsLinearProblem::setObjectiveCoefficient(LinearProblemApi::IMipVariable* var,
                                                   double coefficient)
{
    objective_->SetCoefficient(getMpVar(var), coefficient);
}

double OrtoolsLinearProblem::getObjectiveCoefficient(
  const LinearProblemApi::IMipVariable* var) const
{
    return objective_->GetCoefficient(getMpVar(var));
}

void OrtoolsLinearProblem::setObjectiveOffset(double objectiveOffset)
{
    offsetHandler_->setOffset(objectiveOffset);
}

double OrtoolsLinearProblem::getObjectiveOffset() const
{
    return offsetHandler_->getOffset();
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

bool OrtoolsLinearProblem::isLP() const
{
    return isLP_;
}

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
