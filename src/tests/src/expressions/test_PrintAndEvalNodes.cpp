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

#include <boost/mpl/list.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

BOOST_AUTO_TEST_SUITE(_PrintAndEvalNodes_)

BOOST_AUTO_TEST_CASE(test_getSystemParameterValueAsDouble)
{
    // Mock dependency
    struct MockLinearProblemData: Antares::Optimisation::LinearProblemApi::ILinearProblemData
    {
        double getData(const std::string&, const std::string&, unsigned int, unsigned int) override
        {
            return 123.45; // Mock return value for testing
        }
    } mockData;

    std::map<std::string, ParameterTypeAndValue> system_parameters = {
      {"valid_number", {"valid_number", ParameterType::CONSTANT, "42.5"}},
      {"invalid_number", {"invalid_number", ParameterType::CONSTANT, "abc"}},
      {"out_of_range", {"out_of_range", ParameterType::CONSTANT, "1e500"}},
      {"timeserie_param", {"timeserie_param", ParameterType::TIMESERIE, "timeserie_file"}}};

    std::map<std::string, double> variables; // Not needed for this test

    EvaluationContext context(system_parameters, variables, mockData);

    // 1. Valid number (CONSTANT)
    BOOST_CHECK_EQUAL(context.getSystemParameterValueAsDouble("valid_number"), 42.5);

    // 2. Parameter not found
    BOOST_CHECK_THROW(context.getSystemParameterValueAsDouble("nonexistent"),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::out_of_range>);

    // 3. Invalid number format
    BOOST_CHECK_THROW(context.getSystemParameterValueAsDouble("invalid_number"),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::invalid_argument>);

    // 4. Out of range value
    BOOST_CHECK_THROW(context.getSystemParameterValueAsDouble("out_of_range"),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::out_of_range>);

    // 5. Timeserie parameter should NOT be converted to double, expect an exception
    BOOST_CHECK_THROW(context.getSystemParameterValueAsDouble("timeserie_param"),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::invalid_argument>);

    // 6. Timeserie parameter should be handled by getParameterValue instead
    BOOST_CHECK_EQUAL(context.getParameterValue("timeserie_param", "group1", 0, 1), 123.45);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_ConstructorTest)
{
    EvaluationResult res1(5.0);
    BOOST_CHECK_EQUAL(std::get<double>(res1.value()), 5.0);

    std::vector<double> vec = {1.0, 2.0, 3.0};
    EvaluationResult res2(vec);
    BOOST_CHECK(std::get<std::vector<double>>(res2.value()) == vec);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorAdditionTest)
{
    EvaluationResult res1(5.0);
    EvaluationResult res2(3.0);
    EvaluationResult res3 = res1 + res2;
    BOOST_CHECK_EQUAL(std::get<double>(res3.value()), 8.0);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorSubtractionTest)
{
    EvaluationResult res1(5.0);
    EvaluationResult res2(3.0);
    EvaluationResult res3 = res1 - res2;
    BOOST_CHECK_EQUAL(std::get<double>(res3.value()), 2.0);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorMultiplicationTest)
{
    EvaluationResult res1(5.0);
    EvaluationResult res2(3.0);
    EvaluationResult res3 = res1 * res2;
    BOOST_CHECK_EQUAL(std::get<double>(res3.value()), 15.0);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorDivisionTest)
{
    EvaluationResult res1(6.0);
    EvaluationResult res2(2.0);
    EvaluationResult res3 = res1 / res2;
    BOOST_CHECK_EQUAL(std::get<double>(res3.value()), 3.0);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_DivisionByZeroTest)
{
    EvaluationResult res1(5.0);
    EvaluationResult res2(0.0);
    BOOST_CHECK_THROW(res1 / res2, EvalVisitorDivisionException);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorNegationTest)
{
    EvaluationResult res1(5.0);
    EvaluationResult res2 = -res1;
    BOOST_CHECK_EQUAL(std::get<double>(res2.value()), -5.0);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_VectorOperationTest)
{
    std::vector<double> vec1 = {1.0, 2.0, 3.0};
    std::vector<double> vec2 = {4.0, 5.0, 6.0};
    EvaluationResult res1(vec1);
    EvaluationResult res2(vec2);
    EvaluationResult res3 = res1 + res2;
    const auto expected_result = std::vector<double>{5.0, 7.0, 9.0};
    const auto result = std::get<std::vector<double>>(res3.value());
    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(),
                                  result.cend(),
                                  expected_result.cbegin(),
                                  expected_result.cend());
}

BOOST_AUTO_TEST_CASE(EvaluationResult_MismatchedVectorSizeTest)
{
    std::vector<double> vec1 = {1.0, 2.0};
    std::vector<double> vec2 = {3.0, 4.0, 5.0};
    EvaluationResult res1(vec1);
    EvaluationResult res2(vec2);
    BOOST_CHECK_THROW(res1 + res2, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_ScalarVectorAdditionTest)
{
    EvaluationResult res1(2.0);
    std::vector<double> vec = {1.0, 2.0, 3.0};
    EvaluationResult res2(vec);
    EvaluationResult res3 = res1 + res2;
    const auto expected_result = std::vector<double>{3.0, 4.0, 5.0};
    const auto result = std::get<std::vector<double>>(res3.value());
    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(),
                                  result.cend(),
                                  expected_result.cbegin(),
                                  expected_result.cend());
}

BOOST_AUTO_TEST_CASE(EvaluationResult_ScalarVectorMultiplicationTest)
{
    EvaluationResult res1(2.0);
    std::vector<double> vec = {1.0, 2.0, 3.0};
    EvaluationResult res2(vec);
    EvaluationResult res3 = res1 * res2;
    const auto expected_result = std::vector<double>{2.0, 4.0, 6.0};
    const auto result = res3.valuesAsVector();
    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(),
                                  result.cend(),
                                  expected_result.cbegin(),
                                  expected_result.cend());
}

BOOST_AUTO_TEST_CASE(EvaluationResult_VectorVectorMultiplicationTest)
{
    std::vector<double> vec1 = {1.0, 2.0, 3.0};
    std::vector<double> vec2 = {4.0, 5.0, 6.0};
    EvaluationResult res1(vec1);
    EvaluationResult res2(vec2);
    EvaluationResult res3 = res1 * res2;
    const auto expected_result = std::vector<double>{4.0, 10.0, 18.0};
    const auto result = res3.valuesAsVector();
    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(),
                                  result.cend(),
                                  expected_result.cbegin(),
                                  expected_result.cend());
}

BOOST_AUTO_TEST_CASE(EvaluationResult_VectorScalarDivisionTest)
{
    std::vector<double> vec = {4.0, 8.0, 12.0};
    EvaluationResult res1(vec);
    EvaluationResult res2(2.0);
    EvaluationResult res3 = res1 / res2;
    const auto expected_result = std::vector<double>{2.0, 4.0, 6.0};
    const auto result = std::get<std::vector<double>>(res3.value());
    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(),
                                  result.cend(),
                                  expected_result.cbegin(),
                                  expected_result.cend());
}

struct MyDummyFixture: Registry<Node>
{
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    EvaluationContext evaluationContext{{}, {}, data};
    Antares::Optimisation::LinearProblemApi::FillContext fillContext{0, 0};
    EvalVisitor evalVisitor{evaluationContext, fillContext};
};

BOOST_AUTO_TEST_CASE(print_single_literal)
{
    LiteralNode literal(21);

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(&literal);

    BOOST_CHECK_EQUAL(printed, "21.000000"); // TODO Number of decimals implementation dependent ?
}

BOOST_FIXTURE_TEST_CASE(eval_single_literal, MyDummyFixture)
{
    LiteralNode literal(21);

    const double eval = evalVisitor.dispatch(&literal).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 21.); // TODO Number of decimals implementation dependent ?
}

/*
Fixtures are used for the Registry that manages the memory allocations of nodes

So instead of writing
   Registry<Node> mem;
   Node* root = mem.create<SumNode>(mem.create<LiteralNode>(21), mem.create<LiteralNode>(2));
We can just write
   Node* root = create<SumNode>(create<LiteralNode>(21), create<LiteralNode>(2));

since create<T> is short for this->create<T>.
*/

BOOST_FIXTURE_TEST_CASE(print_add_two_literals, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(21), create<LiteralNode>(2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(printed,
                      "(21.000000+2.000000)"); // TODO Number of decimals implementation dependent ?
}

BOOST_FIXTURE_TEST_CASE(print_add_one_literal, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(215));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(printed,
                      "(215.000000)"); // TODO Number of decimals implementation dependent ?
}

BOOST_FIXTURE_TEST_CASE(print_add_zero_literal, MyDummyFixture)
{
    Node* root = create<SumNode>();

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(printed, "()");
}

BOOST_FIXTURE_TEST_CASE(print_add_six_literals, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(21),
                                 create<LiteralNode>(2),
                                 create<LiteralNode>(34),
                                 create<LiteralNode>(56),
                                 create<LiteralNode>(12),
                                 create<LiteralNode>(86));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(root);

    BOOST_CHECK_EQUAL(
      printed,
      "(21.000000+2.000000+34.000000+56.000000+12.000000+86.000000)"); // TODO Number of decimals
                                                                       // implementation dependent ?
}

BOOST_FIXTURE_TEST_CASE(eval_add_two_literals, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(21), create<LiteralNode>(2));
    double eval = evalVisitor.dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 23.);
}

BOOST_FIXTURE_TEST_CASE(eval_add_one_literal, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(215));

    double eval = evalVisitor.dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 215.);
}

BOOST_FIXTURE_TEST_CASE(eval_add_zero_literal, MyDummyFixture)
{
    Node* root = create<SumNode>();

    double eval = evalVisitor.dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 0.);
}

BOOST_FIXTURE_TEST_CASE(eval_add_six_literals, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(21),
                                 create<LiteralNode>(2),
                                 create<LiteralNode>(34),
                                 create<LiteralNode>(56),
                                 create<LiteralNode>(12),
                                 create<LiteralNode>(86));

    double eval = evalVisitor.dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 211.);
}

BOOST_FIXTURE_TEST_CASE(eval_negation_literal, MyDummyFixture)
{
    const double num = 1428.0;
    Node* root = create<NegationNode>(create<LiteralNode>(num));
    double eval = evalVisitor.dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, -num);
}

BOOST_FIXTURE_TEST_CASE(eval_Add_And_Negation_Nodes, MyDummyFixture)
{
    const double num1 = 1428;
    const double num2 = 8241;
    Node* negative_num2 = create<NegationNode>(create<LiteralNode>(num2));
    Node* root = create<SumNode>(create<LiteralNode>(num1), negative_num2);
    double eval = evalVisitor.dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, num1 - num2);
}

BOOST_FIXTURE_TEST_CASE(Negative_of_SumNode, MyDummyFixture)
{
    const double num1 = 1428;
    const double num2 = 8241;
    Node* add_node = create<SumNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));
    Node* neg = create<NegationNode>(add_node);
    double eval = evalVisitor.dispatch(neg).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, -(num1 + num2));
}

BOOST_FIXTURE_TEST_CASE(print_port_field_node, MyDummyFixture)
{
    PortFieldNode pt_fd("august", "2024");
    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(&pt_fd);

    BOOST_CHECK_EQUAL(printed, "august.2024");
}

BOOST_FIXTURE_TEST_CASE(print_port_field_sum_node, MyDummyFixture)
{
    PortFieldSumNode pt_fd("august", "2024");
    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(&pt_fd);

    BOOST_CHECK_EQUAL(printed, "august.2024");
}

std::pair<std::string, ParameterTypeAndValue> build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const ParameterType& type = ParameterType::CONSTANT)
{
    return {id, {.id = id, .type = type, .value = value}};
}

BOOST_FIXTURE_TEST_CASE(evaluate_param, MyDummyFixture)
{
    ParameterNode root("my-param", TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    const std::string value = "221.3";
    EvaluationContext context({build_context_parameter_with("my-param", value)}, {}, data);

    EvalVisitor evalVisitor(context, fillContext);
    const double eval = evalVisitor.dispatch(&root).valueAsDouble();

    BOOST_CHECK_EQUAL(std::stod(value), eval);
}

struct MockLinearProblemData: Antares::Optimisation::LinearProblemApi::ILinearProblemData
{
    double getData(const std::string& dataSetId,
                   const std::string& scenarioGroup,
                   const unsigned scenario,
                   const unsigned hour) override
    {
        return hour; // for test
    }
};

BOOST_FIXTURE_TEST_CASE(evaluate_time_dependent_param, MyDummyFixture)
{
    ParameterNode root("my-param", TimeIndex::VARYING_IN_TIME_ONLY);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    EvaluationContext context(
      {build_context_parameter_with("my-param", value, ParameterType::TIMESERIE)},
      {},
      dummy_data);

    unsigned hour_0 = 0;
    unsigned hour_1 = 1;
    EvalVisitor evalVisitor(context, {hour_0, hour_1 /*two hours*/});
    const auto eval = evalVisitor.dispatch(&root).valuesAsVector();

    BOOST_CHECK_EQUAL(eval[0], hour_0);
    BOOST_CHECK_EQUAL(eval[1], hour_1);
}

BOOST_FIXTURE_TEST_CASE(evaluate_time_dependent_multiplication, MyDummyFixture)
{
    ParameterNode param("my-param", TimeIndex::VARYING_IN_TIME_ONLY);
    LiteralNode literal(2.0);
    MultiplicationNode root(&literal, &param);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    EvaluationContext context(
      {build_context_parameter_with("my-param", value, ParameterType::TIMESERIE)},
      {},
      dummy_data);

    unsigned hour_0 = 0;
    unsigned hour_1 = 1;
    EvalVisitor evalVisitor(context, {hour_0, hour_1 /*two hours*/});
    const auto eval = evalVisitor.dispatch(&root).valuesAsVector();

    BOOST_CHECK_EQUAL(eval[0], hour_0 * literal.value());
    BOOST_CHECK_EQUAL(eval[1], hour_1 * literal.value());
}

namespace bdata = boost::unit_test::data;
// Helper function to compute the expected value based on the operation type
template<typename BinaryNode>
double evalExpected(double a, double b);

template<>
double evalExpected<MultiplicationNode>(double a, double b)
{
    return a * b;
}

template<>
double evalExpected<SumNode>(double a, double b)
{
    return a + b;
}

template<>
double evalExpected<SubtractionNode>(double a, double b)
{
    return a - b;
}

template<>
double evalExpected<DivisionNode>(double a, double b)
{
    return b != 0 ? a / b : 0;
} // Handle division by zero

template<typename BinaryNode>
void evaluate_time_dependent_operation()
{
    ParameterNode param("my-param", TimeIndex::VARYING_IN_TIME_ONLY);
    LiteralNode literal(2.0);
    BinaryNode root(&literal, &param); // Correctly use the type as a template argument
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;

    EvaluationContext context(
      {build_context_parameter_with("my-param", value, ParameterType::TIMESERIE)},
      {},
      dummy_data);
    unsigned hour_0 = 0;
    unsigned hour_1 = 1;
    EvalVisitor evalVisitor(context, {hour_0, hour_1 /*two hours*/});
    const auto eval = evalVisitor.dispatch(&root).valuesAsVector();

    BOOST_CHECK_EQUAL(eval[0], evalExpected<BinaryNode>(hour_0, literal.value()));
    BOOST_CHECK_EQUAL(eval[1], evalExpected<BinaryNode>(hour_1, literal.value()));
}

// Define a list of types (not instances)
using BinaryOperators = boost::mpl::
  list<MultiplicationNode, SumNode, SubtractionNode, DivisionNode>;

// Parametrize the test with types
BOOST_AUTO_TEST_CASE_TEMPLATE(evaluate_time_dependent_operations, T, BinaryOperators)
{
    evaluate_time_dependent_operation<T>();
}

BOOST_FIXTURE_TEST_CASE(evaluate_variable, MyDummyFixture)
{
    VariableNode root("my-variable", TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    const double value = 221.3;
    EvaluationContext context({}, {{"my-variable", value}}, data);

    EvalVisitor evalVisitor(context, fillContext);
    const double eval = evalVisitor.dispatch(&root).valueAsDouble();

    BOOST_CHECK_EQUAL(value, eval);
}

BOOST_FIXTURE_TEST_CASE(multiplication_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* mult = create<MultiplicationNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(mult);

    BOOST_CHECK_EQUAL(printed, "(22.000000*8.000000)");
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(mult).valueAsDouble(), num1 * num2);
}

BOOST_FIXTURE_TEST_CASE(division_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* div = create<DivisionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(div);

    BOOST_CHECK_EQUAL(printed, "(22.000000/8.000000)");
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(div).valueAsDouble(), num1 / num2);
}

BOOST_FIXTURE_TEST_CASE(division_by_zero, MyDummyFixture)
{
    double num1 = 22.0, num2 = 0.;
    Node* div = create<DivisionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(div);

    BOOST_CHECK_EQUAL(printed, "(22.000000/0.000000)");

    BOOST_CHECK_THROW(evalVisitor.dispatch(div).valueAsDouble(), EvalVisitorDivisionException);
}

BOOST_FIXTURE_TEST_CASE(DivisionNodeFull, MyDummyFixture)
{
    LiteralNode literalNode1(23.);
    LiteralNode literalNode2(-23.);

    DivisionNode divisionNode1(&literalNode1, &literalNode1);
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(&divisionNode1).valueAsDouble(), 1.0);

    DivisionNode divisionNode2(&literalNode1, &literalNode2);
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(&divisionNode2).valueAsDouble(), -1.0);

    LiteralNode* literalNull = nullptr;

    DivisionNode divisionNode3(&literalNode1, literalNull);

    BOOST_CHECK_THROW(evalVisitor.dispatch(&divisionNode3).valueAsDouble(), InvalidNode);

    // truncated to zero
    LiteralNode literalVerySmall(1.e-324);

    DivisionNode divisionNode4(&literalNode1, &literalVerySmall);

    BOOST_CHECK_THROW(evalVisitor.dispatch(&divisionNode4).valueAsDouble(),
                      EvalVisitorDivisionException);
}

BOOST_FIXTURE_TEST_CASE(subtraction_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* sub = create<SubtractionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(sub);

    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)");
    BOOST_CHECK_EQUAL(evalVisitor.dispatch(sub).valueAsDouble(), num1 - num2);
}

BOOST_FIXTURE_TEST_CASE(comparison_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    // (num1-num2) = (22.000000-8.000000)
    SubtractionNode sub1(create<LiteralNode>(num1), create<LiteralNode>(num2));
    // (num2-num1) = (8.000000-22.000000)
    SubtractionNode sub2(create<LiteralNode>(num2), create<LiteralNode>(num1));

    PrintVisitor printVisitor;

    EqualNode equ(&sub1, &sub2);
    auto printed = printVisitor.dispatch(&equ);
    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)==(8.000000-22.000000)");

    LessThanOrEqualNode lt(&sub1, &sub2);
    printed = printVisitor.dispatch(&lt);
    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)<=(8.000000-22.000000)");

    GreaterThanOrEqualNode gt(&sub1, &sub2);
    printed = printVisitor.dispatch(&gt);
    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)>=(8.000000-22.000000)");
}

BOOST_FIXTURE_TEST_CASE(invalidNode, MyDummyFixture)
{
    SumNode* null_node = nullptr;
    BOOST_CHECK_THROW(evalVisitor.dispatch(null_node).valueAsDouble(), InvalidNode);
}

BOOST_FIXTURE_TEST_CASE(NotEvaluableNodes, MyDummyFixture)
{
    LiteralNode literalNode(23.);
    std::string component_id("id");
    std::string name("name");
    std::vector<Node*> nodes = {create<EqualNode>(&literalNode, &literalNode),
                                create<LessThanOrEqualNode>(&literalNode, &literalNode),
                                create<GreaterThanOrEqualNode>(&literalNode, &literalNode),
                                create<PortFieldNode>(name, name),
                                create<PortFieldSumNode>(name, name),
                                create<ComponentParameterNode>(component_id, name),
                                create<ComponentVariableNode>(component_id, name)};
    for (auto* node: nodes)
    {
        BOOST_CHECK_THROW(evalVisitor.dispatch(node).valueAsDouble(), EvalVisitorNotImplemented);
    }
}

BOOST_AUTO_TEST_CASE(PrintVisitor_name)
{
    PrintVisitor printVisitor;
    BOOST_CHECK_EQUAL(printVisitor.name(), "PrintVisitor");
}

BOOST_FIXTURE_TEST_CASE(EvalVisitor_name, MyDummyFixture)
{
    BOOST_CHECK_EQUAL(evalVisitor.name(), "EvalVisitor");
}
BOOST_AUTO_TEST_SUITE_END()
