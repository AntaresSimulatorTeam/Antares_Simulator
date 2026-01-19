// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/CloneVisitor.h>
#include <antares/expressions/visitors/CompareVisitor.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

struct ComparisonFixture
{
    Node* createSimpleExpression(double param);
    Node* createComplexExpression();

    Registry<Node> registry_;
};

Node* ComparisonFixture::createSimpleExpression(double param)
{
    Node* var1 = registry_.create<LiteralNode>(param);
    Node* param1 = registry_.create<ParameterNode>("param1");
    Node* expr = registry_.create<SumNode>(var1, param1);
    return expr;
}

Node* ComparisonFixture::createComplexExpression()
{
    // NOTE : this expression makes no sense, only for testing purposes
    // NOTE2 : Some elements are re-used (e.g simple), this is valid since memory is handled
    // separately (no double free)

    Node* simple = createSimpleExpression(42.);
    Node* neg = registry_.create<NegationNode>(simple);
    Node* mult = registry_.create<MultiplicationNode>(simple, neg);
    Node* comp = registry_.create<ParameterNode>("hello");
    Node* div = registry_.create<DivisionNode>(mult, comp);
    Node* div2 = registry_.create<DivisionNode>(div, simple);
    Node* add = registry_.create<SumNode>(div, div2, neg);
    Node* sub = registry_.create<SubtractionNode>(add, neg);
    Node* cmp = registry_.create<GreaterThanOrEqualNode>(sub, add);
    Node* pf = registry_.create<PortFieldNode>("port", "field");
    Node* pfsum = registry_.create<PortFieldSumNode>("port", "sum");
    Node* equalNode = registry_.create<EqualNode>(pf, pfsum);
    Node* lessThan = registry_.create<LessThanOrEqualNode>(equalNode, pfsum);
    Node* addf = registry_.create<SumNode>(equalNode, lessThan, cmp);
    return addf;
}

BOOST_AUTO_TEST_SUITE(_CompareVisitor_)

BOOST_FIXTURE_TEST_CASE(simple_comparison_to_itself, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr = createSimpleExpression(65.);
    BOOST_CHECK(cmp.dispatch(expr, expr));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_same, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createSimpleExpression(65.);
    Node* expr2 = createSimpleExpression(65.);
    BOOST_CHECK(cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_different, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createSimpleExpression(64.);
    Node* expr2 = createSimpleExpression(65.);
    BOOST_CHECK(!cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_self_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr = createComplexExpression();
    BOOST_CHECK(cmp.dispatch(expr, expr));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_clone_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr = createComplexExpression();
    CloneVisitor cloneVisitor(registry_);
    Node* expr_cloned = cloneVisitor.dispatch(expr);
    BOOST_CHECK(cmp.dispatch(expr, expr_cloned));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createComplexExpression();
    Node* expr2 = createComplexExpression();
    BOOST_CHECK(cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(comparison_to_other_different_complex, ComparisonFixture)
{
    CompareVisitor cmp;
    Node* expr1 = createComplexExpression();
    Node* expr2 = registry_.create<NegationNode>(expr1);
    BOOST_CHECK(!cmp.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(CompareVisitor_name, Registry<Node>)
{
    CompareVisitor compareVisitor;
    BOOST_CHECK_EQUAL(compareVisitor.name(), "CompareVisitor");
}

BOOST_FIXTURE_TEST_CASE(compare_empty_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* expr1 = registry_.create<SumNode>();
    Node* expr2 = registry_.create<SumNode>();
    BOOST_CHECK(compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_different_size_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2);
    Node* expr2 = registry_.create<SumNode>(literal1);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_different_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* literal3 = registry_.create<LiteralNode>(3.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2);
    Node* expr2 = registry_.create<SumNode>(literal1, literal3);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_same_sums, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* literal3 = registry_.create<LiteralNode>(3.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2, literal3);
    Node* expr2 = registry_.create<SumNode>(literal1, literal2, literal3);
    BOOST_CHECK(compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_same_sums_different_nodes, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(2.);
    Node* literal3 = registry_.create<LiteralNode>(2.);
    Node* expr1 = registry_.create<SumNode>(literal1, literal2);
    Node* expr2 = registry_.create<SumNode>(literal1, literal3);
    BOOST_CHECK(compareVisitor.dispatch(expr1, expr2));
}

BOOST_FIXTURE_TEST_CASE(compare_sum_to_literal, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* expr1 = registry_.create<SumNode>(literal1);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, literal1));
}

BOOST_FIXTURE_TEST_CASE(compare_TimeIndex, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* param1 = registry_.create<ParameterNode>("value");
    Node* expr1 = registry_.create<TimeIndexNode>(literal1, param1);
    BOOST_CHECK(!compareVisitor.dispatch(expr1, literal1));

    CloneVisitor clone_visitor(registry_);
    const auto clone = clone_visitor.dispatch(expr1);
    BOOST_CHECK(compareVisitor.dispatch(expr1, clone));
}

BOOST_FIXTURE_TEST_CASE(compare_TimeShift, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* literal1 = registry_.create<LiteralNode>(1.);
    Node* literal2 = registry_.create<LiteralNode>(-65);
    Node* expr1 = registry_.create<TimeShiftNode>(literal1, literal2);

    CloneVisitor clone_visitor(registry_);
    const auto clone = clone_visitor.dispatch(expr1);
    BOOST_CHECK(compareVisitor.dispatch(expr1, clone));
}

BOOST_FIXTURE_TEST_CASE(compare_dual, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* dual1 = registry_.create<FunctionNode>(FunctionNodeType::dual,
                                                 registry_.create<ParameterNode>("constraint1"),
                                                 registry_.create<LiteralNode>(0));
    Node* dual2 = registry_.create<FunctionNode>(FunctionNodeType::dual,
                                                 registry_.create<ParameterNode>("constraint2"),
                                                 registry_.create<LiteralNode>(1));

    CloneVisitor clone_visitor(registry_);
    const auto clone = clone_visitor.dispatch(dual1);
    const auto* n = dynamic_cast<FunctionNode*>(clone);
    BOOST_CHECK(compareVisitor.dispatch(dual1, clone));
    BOOST_CHECK(!compareVisitor.dispatch(dual1, dual2));
}

BOOST_FIXTURE_TEST_CASE(compare_reducedCost, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* reducedCost1 = registry_.create<FunctionNode>(FunctionNodeType::reduced_cost,
                                                        registry_.create<VariableNode>("var1", 0));
    Node* reducedCost2 = registry_.create<FunctionNode>(FunctionNodeType::reduced_cost,
                                                        registry_.create<VariableNode>("var2", 1));

    CloneVisitor clone_visitor(registry_);
    const auto clone = clone_visitor.dispatch(reducedCost1);
    BOOST_CHECK(compareVisitor.dispatch(reducedCost1, clone));
    BOOST_CHECK(!compareVisitor.dispatch(reducedCost1, reducedCost2));
}

BOOST_FIXTURE_TEST_CASE(compare_pow, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* pow1 = registry_.create<FunctionNode>(FunctionNodeType::pow,
                                                registry_.create<VariableNode>("var1", 0),
                                                registry_.create<LiteralNode>(2));
    Node* pow2 = registry_.create<FunctionNode>(FunctionNodeType::pow,
                                                registry_.create<VariableNode>("var2", 1),
                                                registry_.create<LiteralNode>(1722));

    CloneVisitor clone_visitor(registry_);
    const auto clone = clone_visitor.dispatch(pow1);
    BOOST_CHECK(compareVisitor.dispatch(pow1, clone));
    BOOST_CHECK(!compareVisitor.dispatch(pow1, pow2));
}

BOOST_FIXTURE_TEST_CASE(compare_max, ComparisonFixture)
{
    CompareVisitor compareVisitor;
    Node* max1 = registry_.create<FunctionNode>(FunctionNodeType::max,
                                                registry_.create<VariableNode>("var1", 0),
                                                registry_.create<LiteralNode>(2));
    Node* max2 = registry_.create<FunctionNode>(FunctionNodeType::reduced_cost,
                                                registry_.create<VariableNode>("var2", 1),
                                                registry_.create<LiteralNode>(1722),
                                                registry_.create<ParameterNode>("P"));

    CloneVisitor clone_visitor(registry_);
    const auto clone = clone_visitor.dispatch(max1);
    BOOST_CHECK(compareVisitor.dispatch(max1, clone));
    BOOST_CHECK(!compareVisitor.dispatch(max1, max2));
}

BOOST_FIXTURE_TEST_CASE(comparing_a_floor_node_to_itself, ComparisonFixture)
{
    Node* floorNode = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                     registry_.create<ParameterNode>("p"));

    CompareVisitor compareVisitor;
    BOOST_CHECK(compareVisitor.dispatch(floorNode, floorNode));
}

BOOST_FIXTURE_TEST_CASE(comparing_a_floor_node_to_its_clone, ComparisonFixture)
{
    Node* floorNode = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                     registry_.create<ParameterNode>("p"));
    const auto clonedFloorNode = CloneVisitor(registry_).dispatch(floorNode);

    CompareVisitor compareVisitor;
    BOOST_CHECK(compareVisitor.dispatch(floorNode, clonedFloorNode));
}

BOOST_FIXTURE_TEST_CASE(comparing_a_floor_node_to_a_different_floor_node, ComparisonFixture)
{
    Node* floorNode1 = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                      registry_.create<ParameterNode>("p1"));
    Node* floorNode2 = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                      registry_.create<ParameterNode>("p2"));

    CompareVisitor compareVisitor;
    BOOST_CHECK(!compareVisitor.dispatch(floorNode1, floorNode2));
}

BOOST_FIXTURE_TEST_CASE(comparing_a_ceil_node_to_itself, ComparisonFixture)
{
    Node* ceilNode = registry_.create<FunctionNode>(FunctionNodeType::ceil,
                                                    registry_.create<ParameterNode>("p"));

    CompareVisitor compareVisitor;
    BOOST_CHECK(compareVisitor.dispatch(ceilNode, ceilNode));
}

BOOST_FIXTURE_TEST_CASE(comparing_a_ceil_node_to_its_clone, ComparisonFixture)
{
    Node* ceilNode = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                    registry_.create<ParameterNode>("p"));
    const auto clonedCeilNode = CloneVisitor(registry_).dispatch(ceilNode);

    CompareVisitor compareVisitor;
    BOOST_CHECK(compareVisitor.dispatch(ceilNode, clonedCeilNode));
}

BOOST_FIXTURE_TEST_CASE(comparing_a_ceil_node_to_a_different_floor_node, ComparisonFixture)
{
    Node* floorNode1 = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                      registry_.create<ParameterNode>("p1"));
    Node* floorNode2 = registry_.create<FunctionNode>(FunctionNodeType::floor,
                                                      registry_.create<ParameterNode>("p2"));

    CompareVisitor compareVisitor;
    BOOST_CHECK(!compareVisitor.dispatch(floorNode1, floorNode2));
}

BOOST_AUTO_TEST_SUITE_END()
