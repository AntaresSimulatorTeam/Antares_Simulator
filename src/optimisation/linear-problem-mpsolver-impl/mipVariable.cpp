// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <ortools/linear_solver/linear_solver.h>

#include <antares/optimisation/linear-problem-mpsolver-impl/mipVariable.h>
#include "antares/optimisation/linear-problem-mpsolver-impl/convertOrtoolsBasisStatus.h"

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

OrtoolsMipVariable::OrtoolsMipVariable(operations_research::MPVariable* mpVar):
    mpVar_(mpVar)
{
}

void OrtoolsMipVariable::setLb(double lb)
{
    mpVar_->SetLB(lb);
}

void OrtoolsMipVariable::setUb(double ub)
{
    mpVar_->SetUB(ub);
}

void OrtoolsMipVariable::setBounds(double lb, double ub)
{
    mpVar_->SetBounds(lb, ub);
}

double OrtoolsMipVariable::getLb() const
{
    return mpVar_->lb();
}

double OrtoolsMipVariable::getUb() const
{
    return mpVar_->ub();
}

const operations_research::MPVariable* OrtoolsMipVariable::getMpVar() const
{
    return mpVar_;
}

LinearProblemApi::MipBasisStatus OrtoolsMipVariable::getMipBasisStatus() const
{
    return convertOrtoolsBasisStatus(mpVar_->basis_status());
}

double OrtoolsMipVariable::solutionValue() const
{
    return mpVar_->solution_value();
}

const std::string& OrtoolsMipVariable::getName() const
{
    return mpVar_->name();
}

bool OrtoolsMipVariable::isInteger() const
{
    return mpVar_->integer();
}

double OrtoolsMipVariable::reducedCost() const
{
    return mpVar_->reduced_cost();
}

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
