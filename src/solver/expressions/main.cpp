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
#include <any>
#include <iostream>

#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>
#include <antares/solver/expressions/PrintVisitor.h>
#include "antares/solver/expressions/EvalVisitor.h"

template<class V>
void print(const Antares::Solver::Expressions::Node& node)
{
    V visit;
    std::cout << visit.dispatch(node) << std::endl;
}

int main()
{
    using namespace Antares::Solver::Expressions;
    Node* q = new Add(new Literal(21), new Literal(2));
    print<PrintVisitor>(*q);
    print<EvalVisitor>(*q);
}
