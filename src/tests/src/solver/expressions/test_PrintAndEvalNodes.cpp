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
#include <antares/solver/expressions/visitors/EvalVisitor.h>
#include <antares/solver/expressions/visitors/PrintVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

BOOST_AUTO_TEST_SUITE(_PrintAndEvalNodes_)

BOOST_AUTO_TEST_CASE(print_single_literal)
{
    LiteralNode literal(21);

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(literal);

    BOOST_CHECK_EQUAL(printed, "21.000000"); // TODO Number of decimals implementation dependent ?
}

BOOST_AUTO_TEST_CASE(eval_single_literal)
{
    LiteralNode literal(21);

    EvalVisitor evalVisitor;
    const double eval = evalVisitor.dispatch(literal);

    BOOST_CHECK_EQUAL(eval, 21.); // TODO Number of decimals implementation dependent ?
}

/*
Fixtures are used for the Registry that manages the memory allocations of nodes

So instead of writing
   Registry<Node> mem;
   Node* root = mem.create<AddNode>(mem.create<LiteralNode>(21), mem.create<LiteralNode>(2));
We can just write
   Node* root = create<AddNode>(create<LiteralNode>(21), create<LiteralNode>(2));

since create<T> is short for this->create<T>.
*/

BOOST_FIXTURE_TEST_CASE(print_add_two_literals, Registry<Node>)
{
    Node* root = create<AddNode>(create<LiteralNode>(21), create<LiteralNode>(2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(*root);

    BOOST_CHECK_EQUAL(printed,
                      "(21.000000+2.000000)"); // TODO Number of decimals implementation dependent ?
}

BOOST_FIXTURE_TEST_CASE(eval_add_two_literals, Registry<Node>)
{
    Node* root = create<AddNode>(create<LiteralNode>(21), create<LiteralNode>(2));
    EvalVisitor evalVisitor;
    double eval = evalVisitor.dispatch(*root);

    BOOST_CHECK_EQUAL(eval, 23.);
}

BOOST_FIXTURE_TEST_CASE(eval_negation_literal, Registry<Node>)
{
    const double num = 1428.0;
    Node* root = create<NegationNode>(create<LiteralNode>(num));
    EvalVisitor evalVisitor;
    double eval = evalVisitor.dispatch(*root);

    BOOST_CHECK_EQUAL(eval, -num);
}

BOOST_FIXTURE_TEST_CASE(eval_Add_And_Negation_Nodes, Registry<Node>)
{
    const double num1 = 1428;
    const double num2 = 8241;
    Node* negative_num2 = create<NegationNode>(create<LiteralNode>(num2));
    Node* root = create<AddNode>(create<LiteralNode>(num1), negative_num2);
    EvalVisitor evalVisitor;
    double eval = evalVisitor.dispatch(*root);

    BOOST_CHECK_EQUAL(eval, num1 - num2);
}

BOOST_FIXTURE_TEST_CASE(Negative_of_AddNode, Registry<Node>)
{
    const double num1 = 1428;
    const double num2 = 8241;
    Node* add_node = create<AddNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));
    Node* neg = create<NegationNode>(add_node);
    EvalVisitor evalVisitor;
    double eval = evalVisitor.dispatch(*neg);

    BOOST_CHECK_EQUAL(eval, -(num1 + num2));
}

BOOST_FIXTURE_TEST_CASE(print_port_field_node, Registry<Node>)
{
    PortFieldNode pt_fd("august", "2024");
    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(pt_fd);

    BOOST_CHECK_EQUAL(printed, "august.2024");
}

BOOST_FIXTURE_TEST_CASE(evaluate_param, Registry<Node>)
{
    ParameterNode root("my-param");
    const double value = 221.3;
    EvaluationContext context({{"my-param", value}}, {});

    EvalVisitor evalVisitor(context);
    const double eval = evalVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(value, eval);
}

BOOST_FIXTURE_TEST_CASE(evaluate_variable, Registry<Node>)
{
    VariableNode root("my-variable");
    const double value = 221.3;
    EvaluationContext context({}, {{"my-variable", value}});

    EvalVisitor evalVisitor(context);
    const double eval = evalVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(value, eval);
}

BOOST_FIXTURE_TEST_CASE(multiplication_node, Registry<Node>)
{
    double num1 = 22.0, num2 = 8;
    Node* mult = create<MultiplicationNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(*mult);

    BOOST_CHECK_EQUAL(printed, "(22.000000*8.000000)");
    EvalVisitor evalVisitor;
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(*mult), num1 * num2);
}

BOOST_FIXTURE_TEST_CASE(division_node, Registry<Node>)
{
    double num1 = 22.0, num2 = 8;
    Node* div = create<DivisionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(*div);

    BOOST_CHECK_EQUAL(printed, "(22.000000/8.000000)");
    EvalVisitor evalVisitor;
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(*div), num1 / num2);
}

BOOST_FIXTURE_TEST_CASE(division_by_zero, Registry<Node>)
{
    double num1 = 22.0, num2 = 0.;
    Node* div = create<DivisionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(*div);

    BOOST_CHECK_EQUAL(printed, "(22.000000/0.000000)");
    EvalVisitor evalVisitor;

    BOOST_CHECK_EXCEPTION(evalVisitor.dispatch(*div),
                          EvalVisitorDivisionException,
                          [](const EvalVisitorDivisionException& ex)
                          { return strcmp(ex.what(), "DivisionNode Division by zero") == 0; });
}

BOOST_FIXTURE_TEST_CASE(subtraction_node, Registry<Node>)
{
    double num1 = 22.0, num2 = 8;
    Node* sub = create<SubtractionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(*sub);

    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)");
    EvalVisitor evalVisitor;
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(*sub), num1 - num2);
}

BOOST_FIXTURE_TEST_CASE(comparison_node, Registry<Node>)
{
    double num1 = 22.0, num2 = 8;
    // (num1-num2) = (22.000000-8.000000)
    SubtractionNode sub1(create<LiteralNode>(num1), create<LiteralNode>(num2));
    // (num2-num1) = (8.000000-22.000000)
    SubtractionNode sub2(create<LiteralNode>(num2), create<LiteralNode>(num1));

    PrintVisitor printVisitor;

    EqualNode equ(&sub1, &sub2);
    auto printed = printVisitor.dispatch(equ);
    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)==(8.000000-22.000000)");

    LessThanOrEqualNode lt(&sub1, &sub2);
    printed = printVisitor.dispatch(lt);
    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)<=(8.000000-22.000000)");

    GreaterThanOrEqualNode gt(&sub1, &sub2);
    printed = printVisitor.dispatch(gt);
    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)>=(8.000000-22.000000)");
}

BOOST_AUTO_TEST_SUITE_END()