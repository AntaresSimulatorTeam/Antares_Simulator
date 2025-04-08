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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_read_linear_constraint_visitor_)

struct MyDummyFixture: Registry<Node>
{
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    EvaluationContext evaluationContext{{}, {}, data};
    std::string componentId = "compo";
    ReadLinearConstraintVisitor visitor{evaluationContext, {0, 0}, componentId};
};

BOOST_FIXTURE_TEST_CASE(test_name, MyDummyFixture)
{
    BOOST_CHECK_EQUAL(visitor.name(), "ReadLinearConstraintVisitor");
}

std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const ParameterType& type = ParameterType::CONSTANT)
{
    return {id, {.id = id, .type = type, .value = value}};
}

BOOST_FIXTURE_TEST_CASE(test_visit_equal_node, MyDummyFixture)
{
    // 5 + var1 = var2 + 3 * var1 - param1(9)  ==> -2 * var1 - var2 = -14
    Node* lhs = create<SumNode>(create<LiteralNode>(5.), create<VariableNode>("var1"));
    Node* rhs = create<SumNode>(create<VariableNode>("var2"),
                                create<MultiplicationNode>(create<LiteralNode>(3.),
                                                           create<VariableNode>("var1")),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<EqualNode>(lhs, rhs);
    EvaluationContext context({build_context_parameter_with("param1", "9.")}, {}, data);
    ReadLinearConstraintVisitor visitor(context, {0, 0}, componentId);
    auto constraint = visitor.dispatch(node)[0];
    BOOST_CHECK_EQUAL(constraint.lb, -14.);
    BOOST_CHECK_EQUAL(constraint.ub, -14.);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.size(), 2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var1", 0, 0)), -2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var2", 0, 0)), -1);
}

BOOST_FIXTURE_TEST_CASE(test_visit_less_than_or_equal_node, MyDummyFixture)
{
    // -9 + var3 <= var1 + 5 * var2 - param1(10)  ==> - var1 - 5 * var2 + var3 <= -1
    Node* lhs = create<SumNode>(create<LiteralNode>(-9.), create<VariableNode>("var3"));
    Node* rhs = create<SumNode>(create<VariableNode>("var1"),
                                create<MultiplicationNode>(create<LiteralNode>(5.),
                                                           create<VariableNode>("var2")),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<LessThanOrEqualNode>(lhs, rhs);
    EvaluationContext context({build_context_parameter_with("param1", "10.")}, {}, data);
    ReadLinearConstraintVisitor visitor(context, {0, 0}, componentId);
    auto constraint = visitor.dispatch(node)[0];
    BOOST_CHECK_EQUAL(constraint.lb, -std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(constraint.ub, -1.);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.size(), 3);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var1", 0, 0)), -1);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var2", 0, 0)), -5);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var3", 0, 0)), 1);
}

BOOST_FIXTURE_TEST_CASE(test_visit_greater_than_or_equal_node, MyDummyFixture)
{
    // 5 + var1 >= var2 + 3 * var1 - param1(9)  ==> -2 * var1 - var2 >= -14
    Node* lhs = create<SumNode>(create<LiteralNode>(5.), create<VariableNode>("var1"));
    Node* rhs = create<SumNode>(create<VariableNode>("var2"),
                                create<MultiplicationNode>(create<LiteralNode>(3.),
                                                           create<VariableNode>("var1")),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<GreaterThanOrEqualNode>(lhs, rhs);
    EvaluationContext context({build_context_parameter_with("param1", "9.")}, {}, data);
    ReadLinearConstraintVisitor visitor(context, {0, 0}, componentId);
    auto constraint = visitor.dispatch(node)[0];
    BOOST_CHECK_EQUAL(constraint.lb, -14);
    BOOST_CHECK_EQUAL(constraint.ub, std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(constraint.coef_per_var.size(), 2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var1", 0, 0)), -2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(FullKey(componentId, "var2", 0, 0)), -1);
}

BOOST_FIXTURE_TEST_CASE(test_visit_illegal_node, MyDummyFixture)
{
    auto lit = create<LiteralNode>(5.);
    std::vector<Node*> illegal_nodes = {create<SumNode>(),
                                        create<SubtractionNode>(lit, lit),
                                        create<MultiplicationNode>(lit, lit),
                                        create<DivisionNode>(lit, lit),
                                        create<NegationNode>(lit),
                                        create<VariableNode>("var"),
                                        create<ParameterNode>("param"),
                                        create<LiteralNode>(5.),
                                        create<PortFieldNode>("port", "field"),
                                        create<PortFieldSumNode>("port", "field"),
                                        create<ComponentVariableNode>("x", "y"),
                                        create<ComponentParameterNode>("x", "y"),
                                        create<TimeShiftNode>(lit, lit),
                                        create<TimeIndexNode>(lit, lit),
                                        create<TimeSumNode>(lit, lit, lit),
                                        create<AllTimeSumNode>(lit)};

    for (Node* node: illegal_nodes)
    {
        BOOST_CHECK_EXCEPTION(visitor.dispatch(node),
                              std::invalid_argument,
                              checkMessage("Root node of a constraint must be a comparator."));
    }
}

BOOST_AUTO_TEST_SUITE_END()
