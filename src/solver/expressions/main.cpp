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

#include <antares/solver/expressions/Add.h>
#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/Negate.h>
#include <antares/solver/expressions/Node.h>
#include <antares/solver/expressions/Parameter.h>
#include <antares/solver/expressions/PrintVisitor.h>
#include <antares/solver/expressions/StringVisitor.h>

template<class V>
std::any visit(std::shared_ptr<Antares::Solver::Expressions::Node> node)
{
    V visitor;
    return node->accept(visitor);
}

int main()
{
    using namespace Antares::Solver::Expressions;
    auto p1 = std::make_shared<Parameter>("hello");
    auto p2 = std::make_shared<Parameter>("world");
    auto neg = std::make_shared<Negate>(p2);
    auto root = std::make_shared<Add>(p1, neg);

    visit<PrintVisitor>(root);

    std::cout << std::endl;
    std::cout << std::any_cast<std::string>(visit<StringVisitor>(root));
    std::cout << std::endl;

    std::any clone = visit<CloneVisitor>(root);
    auto root2 = std::any_cast<std::shared_ptr<Node>>(clone);
    visit<PrintVisitor>(root2);
}
