/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
CloneVisitor::CloneVisitor(Registry<Node>& mem):
    registry_(mem)
{
}

Node* CloneVisitor::visit(const Add& add)
{
    return registry_.create<Add>(dispatch(*add.n1_), dispatch(*add.n2_));
}

Node* CloneVisitor::visit(const Negate& neg)
{
    return registry_.create<Negate>(dispatch(*neg.n_));
}

Node* CloneVisitor::visit(const Parameter& param)
{
    return registry_.create<Parameter>(param.value_);
}

Node* CloneVisitor::visit(const Literal& param)
{
    return registry_.create<Literal>(param.value_);
}
} // namespace Antares::Solver::Expressions
