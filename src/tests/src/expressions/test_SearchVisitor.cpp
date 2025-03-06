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

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/SearchVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

BOOST_AUTO_TEST_SUITE(_SearchVisitor_)

BOOST_FIXTURE_TEST_CASE(basic_search, Registry<Node>)
{
    std::string cpvar_name("var"), cpvar_id("id1");
    std::string cp_para_name("par"), cp_para_id("id2");
    ComponentVariableNode cpv(cpvar_id, cpvar_name);
    ComponentParameterNode cpp(cp_para_id, cp_para_name);
    double num1 = 22.0, num2 = 8., num3 = 77.;
    // (num1+num2)
    Node* edge = create<SumNode>(create<LiteralNode>(num1),
                                 create<LiteralNode>(num2),
                                 create<LiteralNode>(num3));
    // -((num1+num2+num3))
    Node* negative_edge = create<NegationNode>(edge);
    // (-((num1+num2+num3))+id1.var)
    Node* add_node = create<SumNode>(negative_edge, &cpv);
    // (-((-((num1+num2+num3))+id1.var))+id2.par) ==
    // (-((-((22.000000+8.000000+77.000000))+id1.var))+id2.par)
    Node* root = create<SumNode>(create<NegationNode>(add_node), &cpp);

    SearchVisitor searchVisitor;
    auto res = searchVisitor(root, "LiteralNode");
}

BOOST_FIXTURE_TEST_CASE(SearchVisitor_name, Registry<Node>)
{
    SearchVisitor searchVisitor;
    BOOST_CHECK_EQUAL(searchVisitor.name(), "SearchVisitor");
}

BOOST_AUTO_TEST_SUITE_END()
