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

#include <ortools/linear_solver/linear_solver.h>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipConstraint.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipVariable.h>

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

double OrtoolsMipConstraint::getCoefficient(LinearProblemApi::IMipVariable* var)
{
    auto* mpvar = dynamic_cast<OrtoolsMipVariable*>(var);
    if (!mpvar)
    {
        logs.error()
          << "Invalid cast, tried from LinearProblemApi::IMipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }

    return mpConstraint_->GetCoefficient(mpvar->getMpVar());
}

const std::string& OrtoolsMipConstraint::getName() const
{
    return mpConstraint_->name();
}

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
