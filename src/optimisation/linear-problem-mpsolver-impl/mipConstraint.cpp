// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <ortools/linear_solver/linear_solver.h>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipConstraint.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipVariable.h>
#include "antares/optimisation/linear-problem-mpsolver-impl/convertOrtoolsBasisStatus.h"

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

OrtoolsMipConstraint::OrtoolsMipConstraint(operations_research::MPConstraint* mpConstraint):
    mpConstraint_(mpConstraint)
{
}

void OrtoolsMipConstraint::setLb(double lb)
{
    mpConstraint_->SetLB(lb);
}

void OrtoolsMipConstraint::setUb(double ub)
{
    mpConstraint_->SetUB(ub);
}

void OrtoolsMipConstraint::setBounds(double lb, double ub)
{
    mpConstraint_->SetBounds(lb, ub);
}

double OrtoolsMipConstraint::getLb() const
{
    return mpConstraint_->lb();
}

double OrtoolsMipConstraint::getUb() const
{
    return mpConstraint_->ub();
}

void OrtoolsMipConstraint::setCoefficient(LinearProblemApi::IMipVariable* var, double coefficient)
{
    auto* mpvar = dynamic_cast<OrtoolsMipVariable*>(var);
    if (!mpvar)
    {
        logs.error()
          << "Invalid cast, tried from LinearProblemApi::IMipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }

    mpConstraint_->SetCoefficient(mpvar->getMpVar(), coefficient);
}

double OrtoolsMipConstraint::getCoefficient(const LinearProblemApi::IMipVariable* var) const
{
    const auto* mpvar = dynamic_cast<const OrtoolsMipVariable*>(var);
    if (!mpvar)
    {
        logs.error()
          << "Invalid cast, tried from LinearProblemApi::IMipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }

    return mpConstraint_->GetCoefficient(mpvar->getMpVar());
}

double OrtoolsMipConstraint::dual() const
{
    return mpConstraint_->dual_value();
}

const std::string& OrtoolsMipConstraint::getName() const
{
    return mpConstraint_->name();
}

LinearProblemApi::MipBasisStatus OrtoolsMipConstraint::getMipBasisStatus() const
{
    return convertOrtoolsBasisStatus(mpConstraint_->basis_status());
}

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
