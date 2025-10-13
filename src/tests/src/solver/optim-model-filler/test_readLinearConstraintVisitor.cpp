/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include "visitorFixture.hpp"
using namespace Antares::Optimisation;
using namespace Antares::Expressions;
using namespace Antares::ModelerStudy;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

BOOST_AUTO_TEST_SUITE(_read_linear_constraint_visitor_)

BOOST_FIXTURE_TEST_CASE(test_name, VisitorFixture<ReadLinearConstraintVisitor>)
{
    BOOST_CHECK_EQUAL(visitor().name(), "ReadLinearConstraintVisitor");
}

BOOST_FIXTURE_TEST_CASE(test_visit_equal_node, VisitorFixture<ReadLinearConstraintVisitor>)
{
    // 5 + var1 = var2 + 3 * var1 - param1(-2)  ==> -2 * var1 - var2 = -3
    Node* lhs = create<SumNode>(create<LiteralNode>(5.), create<VariableNode>("var1", 0));
    Node* rhs = create<SumNode>(create<VariableNode>("var2", 1),
                                create<MultiplicationNode>(create<LiteralNode>(3.),
                                                           create<VariableNode>("var1", 0)),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<EqualNode>(lhs, rhs);

    auto v = visitor();
    auto constraint = v.dispatch(node);
    BOOST_CHECK_EQUAL(constraint.lb[0], -3.);
    BOOST_CHECK_EQUAL(constraint.ub[0], -3.);
    BOOST_REQUIRE_EQUAL(constraint.coef_per_var.size(), 1); // single timestep
    auto& linExpr = constraint.coef_per_var[0];
    BOOST_REQUIRE_EQUAL(linExpr.size(), 2); // two coeffs
    // var1
    BOOST_CHECK_EQUAL(linExpr[0].first, 0);
    BOOST_CHECK_EQUAL(linExpr[0].second, -2.);
    // var2
    BOOST_CHECK_EQUAL(linExpr[1].first, 1);
    BOOST_CHECK_EQUAL(linExpr[1].second, -1.);
}

BOOST_FIXTURE_TEST_CASE(test_visit_less_than_or_equal_node,
                        VisitorFixture<ReadLinearConstraintVisitor>)
{
    // -9 + var3 <= var1 + 5 * var2 - param1(-2)  ==> - var1 - 5 * var2 + var3 <= 11
    Node* lhs = create<SumNode>(create<LiteralNode>(-9.), create<VariableNode>("var3", 2));
    Node* rhs = create<SumNode>(create<VariableNode>("var1", 0),
                                create<MultiplicationNode>(create<LiteralNode>(5.),
                                                           create<VariableNode>("var2", 1)),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<LessThanOrEqualNode>(lhs, rhs);

    auto v = visitor();
    auto constraint = v.dispatch(node);
    BOOST_CHECK_EQUAL(constraint.lb[0], -std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(constraint.ub[0], 11.);
    BOOST_REQUIRE_EQUAL(constraint.coef_per_var.size(), 1); // single timestep
    auto& linExpr = constraint.coef_per_var[0];
    BOOST_REQUIRE_EQUAL(linExpr.size(), 3); // 3 coeffs
    // var1
    BOOST_CHECK_EQUAL(linExpr[0].first, 0);
    BOOST_CHECK_EQUAL(linExpr[0].second, -1.);
    // var2
    BOOST_CHECK_EQUAL(linExpr[1].first, 1);
    BOOST_CHECK_EQUAL(linExpr[1].second, -5.);
    // var3
    BOOST_CHECK_EQUAL(linExpr[2].first, 2);
    BOOST_CHECK_EQUAL(linExpr[2].second, 1);
}

BOOST_FIXTURE_TEST_CASE(test_visit_greater_than_or_equal_node,
                        VisitorFixture<ReadLinearConstraintVisitor>)
{
    // 5 + var1 >= var2 + 3 * var1 - param1(-2)  ==> -2 * var1 - var2 >= -3
    Node* lhs = create<SumNode>(create<LiteralNode>(5.), create<VariableNode>("var1", 0));
    Node* rhs = create<SumNode>(create<VariableNode>("var2", 1),
                                create<MultiplicationNode>(create<LiteralNode>(3.),
                                                           create<VariableNode>("var1", 0)),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<GreaterThanOrEqualNode>(lhs, rhs);
    auto v = visitor();
    auto constraint = v.dispatch(node);
    BOOST_CHECK_EQUAL(constraint.lb[0], -3.);
    BOOST_CHECK_EQUAL(constraint.ub[0], std::numeric_limits<double>::infinity());

    BOOST_REQUIRE_EQUAL(constraint.coef_per_var.size(), 1); // single timestep
    auto& linExpr = constraint.coef_per_var[0];
    BOOST_REQUIRE_EQUAL(linExpr.size(), 2); // 2 coeffs
    // var1
    BOOST_CHECK_EQUAL(linExpr[0].first, 0);
    BOOST_CHECK_EQUAL(linExpr[0].second, -2.);
    // var2
    BOOST_CHECK_EQUAL(linExpr[1].first, 1);
    BOOST_CHECK_EQUAL(linExpr[1].second, -1.);
}

BOOST_FIXTURE_TEST_CASE(test_visit_illegal_node, VisitorFixture<ReadLinearConstraintVisitor>)
{
    auto lit = create<LiteralNode>(5.);
    std::vector<Node*> illegal_nodes = {create<SumNode>(),
                                        create<SubtractionNode>(lit, lit),
                                        create<MultiplicationNode>(lit, lit),
                                        create<DivisionNode>(lit, lit),
                                        create<NegationNode>(lit),
                                        create<VariableNode>("var", 99),
                                        create<ParameterNode>("param"),
                                        create<LiteralNode>(5.),
                                        create<PortFieldNode>("port", "field"),
                                        create<PortFieldSumNode>("port", "field"),
                                        create<TimeShiftNode>(lit, lit),
                                        create<TimeIndexNode>(lit, lit),
                                        create<TimeSumNode>(lit, lit, lit),
                                        create<AllTimeSumNode>(lit)};

    for (Node* node: illegal_nodes)
    {
        BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                              Antares::Error::InvalidArgumentError,
                              checkMessage("Root node of a constraint must be a comparator."));
    }
}

BOOST_AUTO_TEST_SUITE_END()
