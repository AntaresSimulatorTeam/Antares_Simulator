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

#include <antares/solver/optim/ortoolsImpl/mipVariable.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

OrtoolsMipVariable::OrtoolsMipVariable(operations_research::MPVariable& mpVar):
    mpVar_(mpVar)
{
}

void OrtoolsMipVariable::setLb(double lb)
{
    mpVar_.SetLB(lb);
}

void OrtoolsMipVariable::setUb(double ub)
{
    mpVar_.SetUB(ub);
}

void OrtoolsMipVariable::setBounds(double lb, double ub)
{
    mpVar_.SetBounds(lb, ub);
}

double OrtoolsMipVariable::getLb()
{
    return mpVar_.lb();
}

double OrtoolsMipVariable::getUb()
{
    return mpVar_.ub();
}

} // namespace Antares::Solver::Optim::OrtoolsImpl
