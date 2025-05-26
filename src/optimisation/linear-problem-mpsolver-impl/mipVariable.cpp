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

#include <antares/optimisation/linear-problem-mpsolver-impl/mipVariable.h>

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

const std::string& OrtoolsMipVariable::getName() const
{
    return mpVar_->name();
}

bool OrtoolsMipVariable::isInteger() const
{
    return mpVar_->integer();
}

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
