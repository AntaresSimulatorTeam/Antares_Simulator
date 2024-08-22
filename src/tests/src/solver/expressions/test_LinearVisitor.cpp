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

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/CompareVisitor.h>
#include <antares/solver/expressions/visitors/LinearStatus.h>
#include <antares/solver/expressions/visitors/LinearityVisitor.h>
#include <antares/solver/expressions/visitors/PrintVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

namespace bdata = boost::unit_test::data;

// Only necessary for BOOST_CHECK_EQUAL
namespace Antares::Solver::Visitors
{
static std::ostream& operator<<(std::ostream& os, LinearStatus s)
{
    switch (s)
    {
    case LinearStatus::CONSTANT:
        return os << "LinearStatus::CONSTANT";
    case LinearStatus::LINEAR:
        return os << "LinearStatus::LINEAR";
    case LinearStatus::NON_LINEAR:
        return os << "LinearStatus::NON_LINEAR";
    default:
        return os << "<unknown>";
    }
}
} // namespace Antares::Solver::Visitors

BOOST_AUTO_TEST_SUITE(_LinearVisitor_)

BOOST_AUTO_TEST_CASE(linear_status_plus)
{
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR + LinearStatus::LINEAR, LinearStatus::LINEAR);
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR + LinearStatus::CONSTANT, LinearStatus::LINEAR);
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR + LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR);

    BOOST_CHECK_EQUAL(LinearStatus::CONSTANT + LinearStatus::CONSTANT, LinearStatus::CONSTANT);
    BOOST_CHECK_EQUAL(LinearStatus::CONSTANT + LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR);

    BOOST_CHECK_EQUAL(LinearStatus::NON_LINEAR + LinearStatus::NON_LINEAR,
                      LinearStatus::NON_LINEAR);
}

BOOST_AUTO_TEST_CASE(linear_status_mult)
{
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR * LinearStatus::LINEAR, LinearStatus::NON_LINEAR);
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR * LinearStatus::CONSTANT, LinearStatus::LINEAR);
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR * LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR);

    BOOST_CHECK_EQUAL(LinearStatus::CONSTANT * LinearStatus::CONSTANT, LinearStatus::CONSTANT);
    BOOST_CHECK_EQUAL(LinearStatus::CONSTANT * LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR);

    BOOST_CHECK_EQUAL(LinearStatus::NON_LINEAR * LinearStatus::NON_LINEAR,
                      LinearStatus::NON_LINEAR);
}

BOOST_AUTO_TEST_CASE(linear_status_divide)
{
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR / LinearStatus::LINEAR, LinearStatus::NON_LINEAR);
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR / LinearStatus::CONSTANT, LinearStatus::LINEAR);
    BOOST_CHECK_EQUAL(LinearStatus::LINEAR / LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR);

    BOOST_CHECK_EQUAL(LinearStatus::CONSTANT / LinearStatus::CONSTANT, LinearStatus::CONSTANT);
    BOOST_CHECK_EQUAL(LinearStatus::CONSTANT / LinearStatus::NON_LINEAR, LinearStatus::NON_LINEAR);

    BOOST_CHECK_EQUAL(LinearStatus::NON_LINEAR / LinearStatus::NON_LINEAR,
                      LinearStatus::NON_LINEAR);
}

static const std::vector<LinearStatus> LinearStatus_ALL = {LinearStatus::LINEAR,
                                                           LinearStatus::NON_LINEAR,
                                                           LinearStatus::CONSTANT};

BOOST_DATA_TEST_CASE(linear_status_minus, bdata::make(LinearStatus_ALL), x)
{
    BOOST_CHECK_EQUAL(x, -x);
}

BOOST_DATA_TEST_CASE(linear_plus_commutative,
                     bdata::make(LinearStatus_ALL) ^ bdata::make(LinearStatus_ALL),
                     x,
                     y)
{
    BOOST_CHECK_EQUAL(x + y, y + x);
}

BOOST_DATA_TEST_CASE(linear_subtract_same_as_plus,
                     bdata::make(LinearStatus_ALL) ^ bdata::make(LinearStatus_ALL),
                     x,
                     y)
{
    BOOST_CHECK_EQUAL(x - y, x + y);
}

BOOST_DATA_TEST_CASE(linear_multiply_commutative,
                     bdata::make(LinearStatus_ALL) ^ bdata::make(LinearStatus_ALL),
                     x,
                     y)
{
    BOOST_CHECK_EQUAL(x * y, y * x);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes_variable_variable_is_linear, Registry<Node>)
{
    PrintVisitor printVisitor;
    LinearityVisitor linearVisitor;

    VariableNode var1("x");
    // variable
    VariableNode var2("y");
    // x==y
    Node* eq = create<EqualNode>(&var1, &var2);
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*eq), "x==y");
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*eq), LinearStatus::LINEAR);
    // x<=y
    Node* lt = create<LessThanOrEqualNode>(&var1, &var2);
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*lt), "x<=y");
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*lt), LinearStatus::LINEAR);
    // x>=y
    Node* gt = create<GreaterThanOrEqualNode>(&var1, &var2);
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*gt), "x>=y");
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*gt), LinearStatus::LINEAR);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes_variable_constant_is_linear, Registry<Node>)
{
    PrintVisitor printVisitor;
    LinearityVisitor linearVisitor;

    VariableNode var1("x");
    // variable
    LiteralNode literal(21.);
    // x==21
    Node* eq = create<EqualNode>(&var1, &literal);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*eq), LinearStatus::LINEAR);
    // x<=21
    Node* lt = create<LessThanOrEqualNode>(&var1, &literal);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*lt), LinearStatus::LINEAR);
    // x>=21
    Node* gt = create<GreaterThanOrEqualNode>(&var1, &literal);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*gt), LinearStatus::LINEAR);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes_constant_constant_is_constant, Registry<Node>)
{
    PrintVisitor printVisitor;
    LinearityVisitor linearVisitor;

    LiteralNode literal1(2.);
    LiteralNode literal2(21.);
    // 2==21
    Node* eq = create<EqualNode>(&literal1, &literal2);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*eq), LinearStatus::CONSTANT);
    // 2<=21
    Node* lt = create<LessThanOrEqualNode>(&literal1, &literal2);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*lt), LinearStatus::CONSTANT);
    // 2>=21
    Node* gt = create<GreaterThanOrEqualNode>(&literal1, &literal2);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*gt), LinearStatus::CONSTANT);
}

BOOST_FIXTURE_TEST_CASE(comparison_nodes_non_lin_constant_is_constant, Registry<Node>)
{
    PrintVisitor printVisitor;
    LinearityVisitor linearVisitor;

    VariableNode var1("x");
    // variable
    VariableNode var2("y");
    MultiplicationNode mult(&var1, &var2);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(mult), LinearStatus::NON_LINEAR);

    AddNode add(&mult, &var1);
    Node* gt = create<GreaterThanOrEqualNode>(&mult, &var2);
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*gt), LinearStatus::NON_LINEAR);
}

BOOST_FIXTURE_TEST_CASE(simple_linear, Registry<Node>)
{
    LiteralNode literalNode1(10.);
    VariableNode var1("x");
    // 10.*x
    Node* u = create<MultiplicationNode>(&literalNode1, &var1);

    LiteralNode literalNode2(20.);
    ComponentVariableNode var2("id", "y");
    // 20.*id.y
    Node* v = create<MultiplicationNode>(&literalNode2, &var2);
    // 10.*x+20.*id.y
    Node* expr = create<AddNode>(u, v);

    PrintVisitor printVisitor;
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*expr), "((10.000000*x)+(20.000000*id.y))");
    LinearityVisitor linearVisitor;
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*expr), LinearStatus::LINEAR);
}

BOOST_FIXTURE_TEST_CASE(simple_not_linear, Registry<Node>)
{
    VariableNode var1("x");
    ComponentVariableNode var2("id", "y");
    // x*id.y
    Node* expr = create<MultiplicationNode>(&var1, &var2);

    PrintVisitor printVisitor;
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*expr), "(x*id.y)");
    LinearityVisitor linearVisitor;
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*expr), LinearStatus::NON_LINEAR);
}

BOOST_FIXTURE_TEST_CASE(simple_linear_division, Registry<Node>)
{
    VariableNode var1("x");
    // constant
    ParameterNode param("y");
    // x/y
    Node* expr = create<DivisionNode>(&var1, &param);

    PrintVisitor printVisitor;
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*expr), "(x/y)");
    LinearityVisitor linearVisitor;
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*expr), LinearStatus::LINEAR);
}

BOOST_FIXTURE_TEST_CASE(simple_non_linear_division, Registry<Node>)
{
    VariableNode var1("x");
    // variable
    VariableNode var2("y");
    // x/y
    Node* expr = create<DivisionNode>(&var1, &var2);

    PrintVisitor printVisitor;
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*expr), "(x/y)");
    LinearityVisitor linearVisitor;
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*expr), LinearStatus::NON_LINEAR);
}

BOOST_FIXTURE_TEST_CASE(simple_constant_expression, Registry<Node>)
{
    PrintVisitor printVisitor;
    LinearityVisitor linearVisitor;
    LiteralNode var1(65.);
    // Parameter
    ParameterNode par("p1");

    // Port field
    PortFieldNode portFieldNode("port", "field");

    // 65.*p1
    Node* mult = create<MultiplicationNode>(&var1, &par);
    // ((65.*p1)+port.field)
    Node* expr = create<AddNode>(mult, &portFieldNode);
    BOOST_CHECK_EQUAL(printVisitor.dispatch(*expr), "((65.000000*p1)+port.field)");
    BOOST_CHECK_EQUAL(linearVisitor.dispatch(*expr), LinearStatus::CONSTANT);
}
BOOST_AUTO_TEST_SUITE_END()
