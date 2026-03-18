// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <limits>
#include <stdexcept>

#include <antares/optimisation/linear-problem-api/StructuredLinearProblem.h>
#include "antares/exception/RuntimeError.hpp"

namespace Antares::Optimisation::LinearProblemApi
{

StructuredMipVariable::StructuredMipVariable(double lb,
                                             double ub,
                                             bool integer,
                                             const std::string& name,
                                             unsigned int index):
    lb_(lb),
    ub_(ub),
    integer_(integer),
    name_(name),
    index_(index)
{
}

void StructuredMipVariable::setLb(double lb)
{
    lb_ = lb;
}

void StructuredMipVariable::setUb(double ub)
{
    ub_ = ub;
}

void StructuredMipVariable::setBounds(double lb, double ub)
{
    lb_ = lb;
    ub_ = ub;
}

double StructuredMipVariable::getLb() const
{
    return lb_;
}

double StructuredMipVariable::getUb() const
{
    return ub_;
}

const std::string& StructuredMipVariable::getName() const
{
    return name_;
}

bool StructuredMipVariable::isInteger() const
{
    return integer_;
}

MipBasisStatus StructuredMipVariable::getMipBasisStatus() const
{
    return MipBasisStatus::NOT_AVAILABLE;
}

double StructuredMipVariable::solutionValue() const
{
    throw Antares::Error::RuntimeError(
      "StructuredMipVariable::solutionValue not available (problem not solved)");
}

double StructuredMipVariable::reducedCost() const
{
    throw Antares::Error::RuntimeError(
      "StructuredMipVariable::reducedCost not available (problem not solved)");
}

unsigned int StructuredMipVariable::index() const
{
    return index_;
}

StructuredMipConstraint::StructuredMipConstraint(double lb,
                                                 double ub,
                                                 const std::string& name,
                                                 unsigned int index):
    lb_(lb),
    ub_(ub),
    name_(name),
    index_(index)
{
}

void StructuredMipConstraint::setLb(double lb)
{
    lb_ = lb;
}

void StructuredMipConstraint::setUb(double ub)
{
    ub_ = ub;
}

void StructuredMipConstraint::setBounds(double lb, double ub)
{
    lb_ = lb;
    ub_ = ub;
}

double StructuredMipConstraint::getLb() const
{
    return lb_;
}

double StructuredMipConstraint::getUb() const
{
    return ub_;
}

void StructuredMipConstraint::setCoefficient(IMipVariable* var, double coefficient)
{
    auto* sv = dynamic_cast<StructuredMipVariable*>(var);
    if (!sv)
    {
        throw std::invalid_argument(
          "StructuredMipConstraint::setCoefficient requires StructuredMipVariable");
    }
    coefficients_[sv->index()] = coefficient;
}

double StructuredMipConstraint::getCoefficient(const IMipVariable* var) const
{
    auto* sv = dynamic_cast<const StructuredMipVariable*>(var);
    if (!sv)
    {
        throw std::invalid_argument(
          "StructuredMipConstraint::getCoefficient requires StructuredMipVariable");
    }
    auto it = coefficients_.find(sv->index());
    if (it == coefficients_.end())
    {
        return 0.0;
    }
    return it->second;
}

std::vector<std::pair<int, double>> StructuredMipConstraint::getCoefficients() const
{
    std::vector<std::pair<int, double>> result;
    result.reserve(coefficients_.size());
    for (const auto& [varIndex, coeff]: coefficients_)
    {
        result.emplace_back(static_cast<int>(varIndex), coeff);
    }
    return result;
}

double StructuredMipConstraint::dual() const
{
    throw Antares::Error::RuntimeError(
      "StructuredMipConstraint::dual not available (problem not solved)");
}

const std::string& StructuredMipConstraint::getName() const
{
    return name_;
}

MipBasisStatus StructuredMipConstraint::getMipBasisStatus() const
{
    return MipBasisStatus::NOT_AVAILABLE;
}

unsigned int StructuredMipConstraint::index() const
{
    return index_;
}

StructuredLinearProblem::StructuredLinearProblem() = default;

IMipVariable* StructuredLinearProblem::addNumVariable(double lb, double ub, const std::string& name)
{
    return addVariable(lb, ub, false, name);
}

IMipVariable* StructuredLinearProblem::addIntVariable(double lb, double ub, const std::string& name)
{
    return addVariable(lb, ub, true, name);
}

IMipVariable* StructuredLinearProblem::addVariable(double lb,
                                                   double ub,
                                                   bool integer,
                                                   const std::string& name)
{
    auto index = static_cast<unsigned int>(variables_.size());
    auto var = std::make_unique<StructuredMipVariable>(lb, ub, integer, name, index);
    variables_.push_back(std::move(var));
    objectiveCoefficients_.push_back(0.0);

    if (integer)
    {
        isMip_ = true;
    }

    return variables_.back().get();
}

const std::vector<std::unique_ptr<IMipVariable>>& StructuredLinearProblem::getVariables() const
{
    return variables_;
}

IMipVariable* StructuredLinearProblem::getVariable(std::size_t index) const
{
    if (index >= variables_.size())
    {
        return nullptr;
    }
    return variables_[index].get();
}

IMipVariable* StructuredLinearProblem::lookupVariable(const std::string& name) const
{
    for (const auto& var: variables_)
    {
        if (var->getName() == name)
        {
            return var.get();
        }
    }
    return nullptr;
}

int StructuredLinearProblem::variableCount() const
{
    return static_cast<int>(variables_.size());
}

IMipConstraint* StructuredLinearProblem::addConstraint(double lb,
                                                       double ub,
                                                       const std::string& name)
{
    auto index = static_cast<unsigned int>(constraints_.size());
    auto cons = std::make_unique<StructuredMipConstraint>(lb, ub, name, index);
    constraints_.push_back(std::move(cons));
    return constraints_.back().get();
}

const std::vector<std::unique_ptr<IMipConstraint>>& StructuredLinearProblem::getConstraints() const
{
    return constraints_;
}

IMipConstraint* StructuredLinearProblem::getConstraint(std::size_t index) const
{
    if (index >= constraints_.size())
    {
        return nullptr;
    }
    return constraints_[index].get();
}

IMipConstraint* StructuredLinearProblem::lookupConstraint(const std::string& name) const
{
    for (const auto& cons: constraints_)
    {
        if (cons->getName() == name)
        {
            return cons.get();
        }
    }
    return nullptr;
}

int StructuredLinearProblem::constraintCount() const
{
    return static_cast<int>(constraints_.size());
}

void StructuredLinearProblem::setObjectiveCoefficient(IMipVariable* var, double coefficient)
{
    auto* sv = dynamic_cast<StructuredMipVariable*>(var);
    if (!sv)
    {
        throw std::invalid_argument(
          "StructuredLinearProblem::setObjectiveCoefficient requires StructuredMipVariable");
    }
    if (sv->index() >= objectiveCoefficients_.size())
    {
        throw std::out_of_range("Variable index out of range in setObjectiveCoefficient");
    }
    objectiveCoefficients_[sv->index()] = coefficient;
}

double StructuredLinearProblem::getObjectiveCoefficient(const IMipVariable* var) const
{
    auto* sv = dynamic_cast<const StructuredMipVariable*>(var);
    if (!sv)
    {
        throw std::invalid_argument(
          "StructuredLinearProblem::getObjectiveCoefficient requires StructuredMipVariable");
    }
    if (sv->index() >= objectiveCoefficients_.size())
    {
        throw std::out_of_range("Variable index out of range in getObjectiveCoefficient");
    }
    return objectiveCoefficients_[sv->index()];
}

void StructuredLinearProblem::setObjectiveOffset(double objectiveOffset)
{
    objectiveOffset_ = objectiveOffset;
}

double StructuredLinearProblem::getObjectiveOffset() const
{
    return objectiveOffset_;
}

void StructuredLinearProblem::setMinimization()
{
    minimize_ = true;
}

void StructuredLinearProblem::setMaximization()
{
    minimize_ = false;
}

bool StructuredLinearProblem::isMinimization() const
{
    return minimize_;
}

bool StructuredLinearProblem::isMaximization() const
{
    return !minimize_;
}

IMipSolution* StructuredLinearProblem::solve(bool)
{
    throw Antares::Error::RuntimeError(
      "StructuredLinearProblem::solve not supported (no solver backend)");
}

double StructuredLinearProblem::infinity() const
{
    return infinity_;
}

bool StructuredLinearProblem::isLP() const
{
    return !isMip_;
}

double StructuredLinearProblem::objectiveValue() const
{
    throw Antares::Error::RuntimeError(
      "StructuredLinearProblem::objectiveValue not available (problem not solved)");
}

} // namespace Antares::Optimisation::LinearProblemApi
