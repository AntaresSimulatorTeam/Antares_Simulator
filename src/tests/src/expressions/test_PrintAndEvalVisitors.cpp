// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <stdexcept>

#include <boost/mpl/list.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include "antares/expressions/ShiftVector.h"

#include "UtilMocks.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation;

BOOST_AUTO_TEST_SUITE(_PrintAndEvalNodes_)

BOOST_AUTO_TEST_CASE(test_getSystemParameterValueAsDouble)
{
    // Mock dependency
    struct MockLinearProblemData: Antares::Optimisation::LinearProblemApi::ILinearProblemData
    {
        [[nodiscard]] double getData(const std::string&, unsigned int, unsigned int) const override
        {
            return 123.45; // Mock return value for testing
        }

        [[nodiscard]] virtual std::span<const double> getData(
          [[maybe_unused]] const std::string& dataSetId,
          [[maybe_unused]] unsigned timeSeriesNumber,
          [[maybe_unused]] unsigned firstHour,
          [[maybe_unused]] unsigned lastHour) const
        {
            static std::vector<double> data = {123.45};
            return data;
        }
    } mockData;

    const auto valid_number = "valid_number";
    const auto invalid_number = "invalid_number";
    const auto out_of_range = "out_of_range";
    const auto scenario_only = "scenario_only";
    const auto timeserie_param = "timeserie_param";
    std::vector<Parameter> params = {
      Parameter{valid_number, TimeDependent::NO, ScenarioDependent::NO},
      Parameter{invalid_number, TimeDependent::NO, ScenarioDependent::NO},
      Parameter{out_of_range, TimeDependent::NO, ScenarioDependent::NO},
      Parameter{scenario_only, TimeDependent::NO, ScenarioDependent::YES},
      Parameter{timeserie_param, TimeDependent::YES, ScenarioDependent::YES}};

    std::map<std::string, ParameterTypeAndValue> system_parameters = {
      {valid_number, {valid_number, VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO, "42.5"}},
      {invalid_number, {invalid_number, VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO, "abc"}},
      {out_of_range, {out_of_range, VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO, "1e500"}},
      {scenario_only, {scenario_only, VariabilityType::VARYING_IN_SCENARIO_ONLY, "timeserie_file"}},
      {timeserie_param,
       {timeserie_param, VariabilityType::VARYING_IN_TIME_ONLY, "timeserie_file"}}};

    std::map<std::string, double> variables; // Not needed for this test

    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;
    auto model = createModelWithParameters(params);
    const std::vector<Component> components{createComponent(model, "compo", system_parameters)};
    EvaluationContext context(&components.back(), &mockData, &emptyScenario);

    // 1. Valid number (CONSTANT)
    BOOST_CHECK_EQUAL(context.getSystemParameterValueAsDouble(valid_number), 42.5);

    // 2. Parameter not found
    BOOST_CHECK_THROW((void)context.getSystemParameterValueAsDouble("nonexistent"),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::out_of_range>);

    // 3. Invalid number format
    BOOST_CHECK_THROW((void)context.getSystemParameterValueAsDouble(invalid_number),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::invalid_argument>);

    // 4. Out of range value
    BOOST_CHECK_THROW((void)context.getSystemParameterValueAsDouble(out_of_range),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::out_of_range>);

    // 5. Timeserie parameter should NOT be converted to double, expect an exception
    BOOST_CHECK_THROW((void)context.getSystemParameterValueAsDouble(timeserie_param),
                      EvaluationContext::CouldNotEvaluateConstantParameter<std::invalid_argument>);

    // 6. Timeserie parameter should be handled by getParameterValue instead
    BOOST_CHECK_EQUAL(context.getParameterValue(timeserie_param, 0, 1), 123.45);

    // 7. Scenario only: handled like a timeserie parameter
    BOOST_CHECK_EQUAL(context.getParameterValue(scenario_only, 0, 1), 123.45);
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

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorNegationOnSingleValue)
{
    EvaluationResult res1(5.0);
    EvaluationResult res2 = -res1;
    BOOST_CHECK_THROW((void)res2.valuesAsVector(), EvalResultTypeError);
    BOOST_CHECK_EQUAL(std::get<double>(res2.value()), -5.0);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_OperatorNegationOnVector)
{
    EvaluationResult res1({5.0, 986.});
    EvaluationResult res2 = -res1;
    BOOST_CHECK_THROW(res2.valueAsDouble(), EvalResultTypeError);

    const std::vector<double> expected_result{-5.0, -986.};

    const auto result = std::get<std::vector<double>>(res2.value());

    BOOST_CHECK_EQUAL_COLLECTIONS(result.cbegin(),
                                  result.cend(),
                                  expected_result.cbegin(),
                                  expected_result.cend());
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

// Equality operator
bool operator==(const EvaluationResult& lhs, const EvaluationResult& rhs)
{
    const auto lhs_value = lhs.value();
    const auto rhs_value = rhs.value();
    if (std::holds_alternative<double>(lhs_value) && std::holds_alternative<double>(rhs_value))
    {
        return std::get<double>(lhs_value) == std::get<double>(rhs_value);
    }
    else if (std::holds_alternative<std::vector<double>>(lhs_value)
             && std::holds_alternative<std::vector<double>>(rhs_value))
    {
        return std::get<std::vector<double>>(lhs_value) == std::get<std::vector<double>>(rhs_value);
    }
    return false;
}

BOOST_AUTO_TEST_CASE(EvaluationResult_operator_bracket)
{
    const std::vector<double> vec = {4.0, 8.0, 12.0};
    const EvaluationResult res1(vec);

    BOOST_CHECK_NO_THROW(res1[0].valueAsDouble());
    BOOST_CHECK_THROW((void)res1[0].valuesAsVector(), EvalResultTypeError);
    BOOST_CHECK_EQUAL(res1[0].valueAsDouble(), vec[0]);
    BOOST_CHECK_EQUAL(res1[1].valueAsDouble(), vec[1]);
    BOOST_CHECK_EQUAL(res1[2].valueAsDouble(), vec[2]);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_operator_bracket_one_value)
{
    const EvaluationResult res1(2025.03);

    BOOST_CHECK_NO_THROW(res1[0].valueAsDouble());
    BOOST_CHECK_THROW((void)res1[0].valuesAsVector(), EvalResultTypeError);
    BOOST_CHECK_THROW((void)res1[0].getValueInVector(0), EvalResultTypeError);
    BOOST_CHECK_EQUAL(res1[0].valueAsDouble(), 2025.03);
    BOOST_CHECK_EQUAL(res1[10].valueAsDouble(), 2025.03);
    BOOST_CHECK_EQUAL(res1[2000].valueAsDouble(), 2025.03);
    BOOST_CHECK_EQUAL(res1[-20000].valueAsDouble(), 2025.03);
    BOOST_CHECK_EQUAL(res1[(int)2025.03].valueAsDouble(), 2025.03);
}

BOOST_AUTO_TEST_CASE(EvaluationResult_invalid_index)
{
    const std::vector<double> vec = {4.0, 8.0, 12.0};
    const EvaluationResult res1(vec);

    for (const int size = static_cast<int>(vec.size());
         const auto& invalidIndex: {-40, size, size + 56})
    {
        BOOST_CHECK_THROW(res1[invalidIndex], EvalResultTimeIndexOutOfRange);
    }
}

BOOST_AUTO_TEST_CASE(ShiftResult_DoubleValue)
{
    const EvaluationResult eval(4.0);
    const EvaluationResult shiftedEval = eval.timeShift(2);
    BOOST_CHECK_THROW((void)eval.timeShift(2).valuesAsVector(), EvalResultTypeError);
    BOOST_CHECK_NO_THROW(eval.timeShift(2).valueAsDouble());

    BOOST_CHECK_EQUAL(eval.timeShift(2).valueAsDouble(), 4.0);
    BOOST_CHECK_EQUAL(eval.timeShift(-2).valueAsDouble(), 4.0);
    BOOST_CHECK_EQUAL(eval.timeShift(0).valueAsDouble(), 4.0);
    BOOST_CHECK_EQUAL(eval.timeShift(-20).valueAsDouble(), 4.0);
    BOOST_CHECK_EQUAL(eval.timeShift(200).valueAsDouble(), 4.0);
}

BOOST_AUTO_TEST_CASE(ShiftResult_VectorValue_PositiveShift)
{
    EvaluationResult eval(std::vector<double>{1.0, 2.0, 3.0});

    BOOST_CHECK_THROW(eval.timeShift(2).valueAsDouble(), EvalResultTypeError);
    BOOST_CHECK_NO_THROW((void)eval.timeShift(2).valuesAsVector());
    const auto res = eval.timeShift(1).valuesAsVector();
    const std::vector<double> expected{2.0, 3.0, 1.0};
    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(ShiftResult_VectorValue_NegativeShift)
{
    const EvaluationResult eval(std::vector<double>{1.0, 2.0, 3.0});
    const auto res = eval.timeShift(-1).valuesAsVector();
    const std::vector<double> expected{3.0, 1.0, 2.0};
    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(ShiftResult_VectorValue_ZeroShift)
{
    std::vector<double> vec{1.0, 2.0, 3.0};
    const EvaluationResult eval(vec);
    const auto res = eval.timeShift(0).valuesAsVector();

    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), vec.begin(), vec.end());
}

BOOST_AUTO_TEST_CASE(ShiftResult_EmptyVector)
{
    std::vector<double> emptyVec{};
    const EvaluationResult eval(emptyVec);
    const auto res = eval.timeShift(1).valuesAsVector();
    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), emptyVec.begin(), emptyVec.end());
}

BOOST_AUTO_TEST_CASE(TimeSum_DoubleValue)
{
    const EvaluationResult eval(4.0);
    const EvaluationResult sum = eval.timeSum(-2, 2);
    BOOST_CHECK_EQUAL(sum.valueAsDouble(), 20.0); // 4.0 * 5 = 20.0

    BOOST_CHECK_THROW((void)eval.timeSum(-1, 0).valuesAsVector(), EvalResultTypeError);
}

BOOST_AUTO_TEST_CASE(TimeSum_VectorValue_PositiveShift)
{
    const EvaluationResult eval(std::vector<double>{1.0, 2.0, 3.0});
    const auto sum = eval.timeSum(0, 2).valuesAsVector();
    BOOST_CHECK_THROW(eval.timeSum(-1, 0).valueAsDouble(), EvalResultTypeError);
    const std::vector<double> expected(3.0, 1.0 + 2.0 + 3.0);

    BOOST_CHECK_EQUAL_COLLECTIONS(sum.begin(), sum.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(TimeSum_VectorValue_NegativeShift)
{
    const EvaluationResult eval(std::vector<double>{1.0, 2.0, 3.0});
    const auto sum = eval.timeSum(-1, 0).valuesAsVector();
    BOOST_CHECK_THROW(eval.timeSum(-1, 0).valueAsDouble(), EvalResultTypeError);

    const std::vector<double> expected{3.0 + 1.0, 1.0 + 2.0, 2.0 + 3.0};

    BOOST_CHECK_EQUAL_COLLECTIONS(sum.begin(), sum.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(AlltimeSum_DoubleValue)
{
    const EvaluationResult eval(4.0);
    const EvaluationResult sum = eval.alltimeSum(5);
    BOOST_CHECK_EQUAL(sum.valueAsDouble(), 20.0); // 4.0 * 5 = 20.0

    BOOST_CHECK_THROW((void)sum.valuesAsVector(), EvalResultTypeError);
}

BOOST_AUTO_TEST_CASE(AlltimeSum_VectorValue)
{
    const EvaluationResult eval(std::vector<double>{1.0, 2.0, 3.0});
    const EvaluationResult sum = eval.alltimeSum(3);
    BOOST_CHECK_EQUAL(sum.valueAsDouble(), 6.0); // 1.0 + 2.0 + 3.0 = 6.0
}

BOOST_AUTO_TEST_CASE(AlltimeSum_VectorValue_OutOfRange)
{
    const EvaluationResult eval(std::vector<double>{1.0, 2.0, 3.0});
    BOOST_CHECK_THROW(eval.alltimeSum(4), EvalResultTimeIndexOutOfRange);
}

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

    const double eval = defaultComponentEvalVisitor->dispatch(&literal).valueAsDouble();

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
    double eval = defaultComponentEvalVisitor->dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 23.);
}

BOOST_FIXTURE_TEST_CASE(eval_add_one_literal, MyDummyFixture)
{
    Node* root = create<SumNode>(create<LiteralNode>(215));

    double eval = defaultComponentEvalVisitor->dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 215.);
}

BOOST_FIXTURE_TEST_CASE(eval_add_zero_literal, MyDummyFixture)
{
    Node* root = create<SumNode>();

    double eval = defaultComponentEvalVisitor->dispatch(root).valueAsDouble();

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

    double eval = defaultComponentEvalVisitor->dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, 211.);
}

BOOST_FIXTURE_TEST_CASE(eval_negation_literal, MyDummyFixture)
{
    const double num = 1428.0;
    Node* root = create<NegationNode>(create<LiteralNode>(num));
    double eval = defaultComponentEvalVisitor->dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, -num);
}

BOOST_FIXTURE_TEST_CASE(eval_Add_And_Negation_Nodes, MyDummyFixture)
{
    const double num1 = 1428;
    const double num2 = 8241;
    Node* negative_num2 = create<NegationNode>(create<LiteralNode>(num2));
    Node* root = create<SumNode>(create<LiteralNode>(num1), negative_num2);
    double eval = defaultComponentEvalVisitor->dispatch(root).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, num1 - num2);
}

BOOST_FIXTURE_TEST_CASE(Negative_of_SumNode, MyDummyFixture)
{
    const double num1 = 1428;
    const double num2 = 8241;
    Node* add_node = create<SumNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));
    Node* neg = create<NegationNode>(add_node);
    double eval = defaultComponentEvalVisitor->dispatch(neg).valueAsDouble();

    BOOST_CHECK_EQUAL(eval, -(num1 + num2));
}

BOOST_FIXTURE_TEST_CASE(comparisonEqualNode_basic, MyDummyFixture)
{
    const double num1 = 1428;
    const double num2 = 8241;

    Node* equalSameValue = create<EqualNode>(create<LiteralNode>(num1), create<LiteralNode>(num1));
    Node* equalDifferentValue = create<EqualNode>(create<LiteralNode>(num1),
                                                  create<LiteralNode>(num2));

    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(equalSameValue).valueAsDouble(), 1.0);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(equalDifferentValue).valueAsDouble(),
                      0.0);
}

BOOST_FIXTURE_TEST_CASE(comparisonEqualNode_complex, MyDummyFixture)
{
    ParameterNode root("my-param", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    const std::string value = "221.3";
    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::NO, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param", value);
    const auto compoName = components.back().Id() + "1245";
    const auto* compo = addComponent(compoName, model, {param});
    EvalVisitor visitor(optimEntityContainer,
                        ctx,
                        *compo,
                        &data,
                        &scenarioGroupRepository.scenario(compo->getScenarioGroupId()));

    const double num = 221.3;
    Node* equalLiteralParam = create<EqualNode>(create<LiteralNode>(num), &root);

    BOOST_CHECK_EQUAL(visitor.dispatch(equalLiteralParam).valueAsDouble(), 1.0);
}

BOOST_FIXTURE_TEST_CASE(comparisonLessThanOrEqualNode, MyDummyFixture)
{
    const double num1 = 1428;
    const double num2 = 8241;

    Node* nodeLessTrue = create<LessThanOrEqualNode>(create<LiteralNode>(num1),
                                                     create<LiteralNode>(num2));
    Node* nodeLessFalse = create<LessThanOrEqualNode>(create<LiteralNode>(num2),
                                                      create<LiteralNode>(num1));
    Node* nodeEqualTrue = create<LessThanOrEqualNode>(create<LiteralNode>(num1),
                                                      create<LiteralNode>(num1));

    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(nodeLessTrue).valueAsDouble(), 1.0);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(nodeLessFalse).valueAsDouble(), 0.0);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(nodeEqualTrue).valueAsDouble(), 1.0);
}

BOOST_FIXTURE_TEST_CASE(comparisonGraterThanOrEqualNode, MyDummyFixture)
{
    const double num1 = 8241;
    const double num2 = 1428;

    Node* nodeLessTrue = create<GreaterThanOrEqualNode>(create<LiteralNode>(num1),
                                                        create<LiteralNode>(num2));
    Node* nodeLessFalse = create<GreaterThanOrEqualNode>(create<LiteralNode>(num2),
                                                         create<LiteralNode>(num1));
    Node* nodeEqualTrue = create<GreaterThanOrEqualNode>(create<LiteralNode>(num1),
                                                         create<LiteralNode>(num1));

    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(nodeLessTrue).valueAsDouble(), 1.0);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(nodeLessFalse).valueAsDouble(), 0.0);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(nodeEqualTrue).valueAsDouble(), 1.0);
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

BOOST_FIXTURE_TEST_CASE(evaluate_param, MyDummyFixture)
{
    ParameterNode root("my-param", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    const std::string value = "221.3";
    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::NO, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param", value);
    const auto compoName = components.back().Id() + "1245";
    const auto* compo = addComponent(compoName, model, {param});

    EvalVisitor visitor(optimEntityContainer,
                        ctx,
                        *compo,
                        &data,
                        &scenarioGroupRepository.scenario(compo->getScenarioGroupId()));

    const double eval = visitor.dispatch(&root).valueAsDouble();

    BOOST_CHECK_EQUAL(std::stod(value), eval);
}

BOOST_FIXTURE_TEST_CASE(evaluate_param_scenario_only, MyDummyFixture)
{
    ParameterNode root("my-param", VariabilityType::VARYING_IN_SCENARIO_ONLY);
    const std::string value = "144.4";
    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::NO, ScenarioDependent::YES)});
    auto param = build_context_parameter_with("my-param", value);
    const auto compoName = components.back().Id() + "1245";
    const auto* compo = addComponent(compoName, model, {param});

    EvalVisitor visitor(optimEntityContainer,
                        ctx,
                        *compo,
                        &data,
                        &scenarioGroupRepository.scenario(compo->getScenarioGroupId()));

    const double eval = visitor.dispatch(&root).valueAsDouble();

    BOOST_CHECK_EQUAL(std::stod(value), eval);
}

struct MockLinearProblemData: Antares::Optimisation::LinearProblemApi::ILinearProblemData
{
    [[nodiscard]] double getData(const std::string& dataSetId,
                                 const unsigned /*scenario*/,
                                 unsigned hour) const override
    {
        if (const auto [ok, value] = IsParameterRegistered(dataSetId, hour); ok)
        {
            return value;
        }
        return hour; // for test
    }

    [[nodiscard]] std::span<const double> getData(const std::string& dataSetId,
                                                  unsigned /*timeSeriesNumber*/,
                                                  unsigned firstHour,
                                                  unsigned lastHour) const override
    {
        if (const auto [ok, value] = IsParameterRegistered(dataSetId); ok)
        {
            lastData_ = value;
            return lastData_;
        }
        lastData_.resize(lastHour - firstHour + 1);
        auto v = firstHour;
        for (std::size_t i = 0; i < lastData_.size(); ++i)
        {
            lastData_[i] = v;
            ++v;
        }
        return lastData_;
    }

    MockLinearProblemData(const std::map<std::string, std::vector<double>>& parametersValues = {}):
        parametersValues(parametersValues)
    {
    }

    std::pair<bool, double> IsParameterRegistered(const std::string& name, unsigned hour) const
    {
        const auto it = std::ranges::find_if(parametersValues,
                                             [&name](const auto& v) { return v.first == name; });
        if (it != parametersValues.cend())
        {
            return std::make_pair(true, it->second[hour]);
        }
        return std::make_pair(false, 0.0);
    }

    std::pair<bool, std::vector<double>> IsParameterRegistered(const std::string& name) const
    {
        const auto it = std::ranges::find_if(parametersValues,
                                             [&name](const auto& v) { return v.first == name; });
        if (it != parametersValues.cend())
        {
            return std::make_pair(true, it->second);
        }
        return std::make_pair(false, std::vector<double>{0});
    }

    void addParams(std::pair<std::string, std::vector<double>> values)
    {
        parametersValues.emplace(values);
    }

    std::map<std::string, std::vector<double>> parametersValues = {};
    mutable std::vector<double> lastData_;
};

struct TimeDependentParameterFixture

{
    ParameterNode paramNode = ParameterNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    unsigned hour_0 = 0;
    unsigned hour_1 = 1;

    Model model;
    std::string compoName = "1245";
    std::vector<Component> components;
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo;
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer = OptimEntityContainer(linearProblem);

    std::unique_ptr<Antares::Expressions::Visitors::EvalVisitor> evalVisitor;
    Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 1, 0, 1, 1};

    TimeDependentParameterFixture(
      std::map<std::string, ParameterTypeAndValue> additionnalParams = {})
    {
        std::vector<Parameter> params{
          Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)};
        for (const auto& [name, typeAndValue]: additionnalParams)
        {
            params.emplace_back(name,
                                typeAndValue.type == VariabilityType::VARYING_IN_TIME_ONLY
                                  ? TimeDependent::YES
                                  : TimeDependent::NO,
                                ScenarioDependent::NO);
        }

        model = createModelWithParameters(params);
        additionnalParams.emplace(
          build_context_parameter_with("my-param", value, VariabilityType::VARYING_IN_TIME_ONLY));
        components.push_back(createComponent(model, compoName, additionnalParams));
        scenarioGroupRepo = makeScenarioGroupRepo(components.front());
        optimContainer.addFromSystemComponents(components);
        evalVisitor = std::make_unique<EvalVisitor>(optimContainer,
                                                    ctx,
                                                    components.front(),
                                                    &dummy_data,
                                                    &scenarioGroupRepo.scenario(
                                                      components.front().getScenarioGroupId()));
    }
};

BOOST_FIXTURE_TEST_CASE(evaluate_time_dependent_param, TimeDependentParameterFixture)
{
    const auto result = evalVisitor->dispatch(&paramNode).valuesAsVector();

    BOOST_CHECK_EQUAL(result[0], hour_0);
    BOOST_CHECK_EQUAL(result[1], hour_1);
}

BOOST_FIXTURE_TEST_CASE(evaluate_shifted_literal, MyDummyFixture)
{
    LiteralNode literal_node(13.0);
    TimeShiftNode time_shift_node(&literal_node, &literal_node);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(&time_shift_node).valueAsDouble(),
                      13.0);
    BOOST_CHECK_THROW(
      (void)defaultComponentEvalVisitor->dispatch(&time_shift_node).valuesAsVector(),
      EvalResultTypeError);
}

template<typename NodeType>
EvaluationResult CreateAndEvaluateTimeNode(Node* p)
{
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    NodeType root(&paramNode, p);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;

    unsigned first = 0;
    unsigned last = 2;

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";
    const std::vector<Component> components{createComponent(model, compoName, {param})};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);
    EvalVisitor visitor(optimContainer,
                        {first, last /*three hours*/, first, last, 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));

    return visitor.dispatch(&root);
}

BOOST_FIXTURE_TEST_CASE(evaluate_shifted_param, MyDummyFixture)
{
    LiteralNode literal_node(-1.0);
    const auto eval = CreateAndEvaluateTimeNode<TimeShiftNode>(&literal_node).valuesAsVector();
    // from MockLinearProblemData  param TSdata is {0, 1, 2}
    // here we applied TimeShift t-1 {2, 0, 1}
    BOOST_CHECK_EQUAL(eval[0], 2); //
    BOOST_CHECK_EQUAL(eval[1], 0);
    BOOST_CHECK_EQUAL(eval[2], 1);
}

BOOST_FIXTURE_TEST_CASE(evaluate_timeIndex_param, MyDummyFixture)
{
    LiteralNode literal_node(1.0);
    const auto eval = CreateAndEvaluateTimeNode<TimeIndexNode>(&literal_node).valueAsDouble();
    // from MockLinearProblemData  param TSdata is {0, 1, 2}
    // here we applied TimeIndex[1]
    BOOST_CHECK_EQUAL(eval, 1); //
}

EvaluationResult CreateAndEvaluateTimeSumNode(Node* from, Node* to)
{
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    TimeSumNode root(from, to, &paramNode);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;

    unsigned first = 0;
    unsigned last = 2;

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";
    const std::vector<Component> components{createComponent(model, compoName, {param})};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);

    EvalVisitor visitor(optimContainer,
                        {first, last /*three hours*/, first, last, 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));

    return visitor.dispatch(&root);
}

BOOST_FIXTURE_TEST_CASE(evaluate_timeSum_param, MyDummyFixture)
{
    LiteralNode from(0.0);
    LiteralNode to(1.0);
    const auto eval = CreateAndEvaluateTimeSumNode(&from, &to).valuesAsVector();
    // from MockLinearProblemData  param TSdata is {0, 1, 2}
    // here we applied TimeSum from t+0 and to t+1
    BOOST_CHECK_EQUAL(eval.at(0), 0 + 1); // add param.at(0)+param.at(1)
    BOOST_CHECK_EQUAL(eval.at(1), 1 + 2); // add param.at(1)+param.at(2)
    BOOST_CHECK_EQUAL(eval.at(2), 2 + 0); // add param.at(2)+param.at(0)
}

EvaluationResult CreateAndEvaluateAllTimeSumNode()
{
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    AllTimeSumNode root(&paramNode);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;

    unsigned first = 0;
    unsigned last = 2;

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";
    const std::vector<Component> components{createComponent(model, compoName, {param})};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);

    EvalVisitor visitor(optimContainer,
                        {first, last /*three hours*/, first, last, 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));
    return visitor.dispatch(&root);
}

BOOST_FIXTURE_TEST_CASE(evaluate_alltimeSum_param, MyDummyFixture)
{
    LiteralNode from(0.0);
    LiteralNode to(1.0);
    const auto eval = CreateAndEvaluateAllTimeSumNode().valueAsDouble();
    // from MockLinearProblemData  param TSdata is {0, 1, 2}
    // here we applied TimeSum from t+0 and to t+1
    BOOST_CHECK_EQUAL(eval, 0 + 1 + 2); // add param.at(0)+param.at(1)
}

BOOST_FIXTURE_TEST_CASE(evaluate_time_dependent_multiplication, MyDummyFixture)
{
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    LiteralNode literal(2.0);
    MultiplicationNode root(&literal, &paramNode);
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    unsigned hour_0 = 0;
    unsigned hour_1 = 1;

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";
    const std::vector<Component> components{createComponent(model, compoName, {param})};
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);

    EvalVisitor visitor(optimContainer,
                        {hour_0, hour_1 /*two hours*/, hour_0, hour_1, 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));
    const auto eval = visitor.dispatch(&root).valuesAsVector();

    BOOST_CHECK_EQUAL(eval[0], hour_0 * literal.value());
    BOOST_CHECK_EQUAL(eval[1], hour_1 * literal.value());
}

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
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    LiteralNode literal(2.0);
    BinaryNode root(&literal, &paramNode); // Correctly use the type as a template argument
    const std::string value = "dummy";
    MockLinearProblemData dummy_data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;
    unsigned hour_0 = 1;
    unsigned hour_1 = 2;

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";

    const std::vector<Component> components{createComponent(model, compoName, {param})};

    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);

    EvalVisitor visitor(optimContainer,
                        {hour_0, hour_1 /*three hours*/, hour_0, hour_1, 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));
    const auto eval = visitor.dispatch(&root).valuesAsVector();

    BOOST_CHECK_EQUAL(eval[0], evalExpected<BinaryNode>(literal.value(), hour_0));
    BOOST_CHECK_EQUAL(eval[1], evalExpected<BinaryNode>(literal.value(), hour_1));
}

template<typename BinaryNode>
void evaluate_time_dependent_operation_on_TimeShiftNode(Node* timeShift)
{
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    LiteralNode literal(2.0);
    BinaryNode binary_node(&literal, &paramNode); // Correctly use the type as a template argument

    TimeShiftNode root(&binary_node, timeShift);
    const std::string value = "dummy";

    MockLinearProblemData dummy_data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;
    std::vector<unsigned int> hours = {1, 2};

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";
    const std::vector<Component> components{createComponent(model, compoName, {param})};

    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);
    EvalVisitor visitor(optimContainer,
                        {hours.at(0), hours.at(1) /*two hours*/, hours.at(0), hours.at(1), 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));
    const auto eval = visitor.dispatch(&root).valuesAsVector();

    std::vector<double> result_before_timeShift = {evalExpected<BinaryNode>(literal.value(),
                                                                            hours.at(0)),
                                                   evalExpected<BinaryNode>(literal.value(),
                                                                            hours.at(1))};

    const auto evaluatedtimeShift = static_cast<int>(visitor.dispatch(timeShift).valueAsDouble());
    const auto after_timeShift = shiftVector(result_before_timeShift, evaluatedtimeShift);
    BOOST_CHECK_EQUAL(eval[0], after_timeShift.at(0));
    BOOST_CHECK_EQUAL(eval[1], after_timeShift.at(1));
}

template<typename BinaryNode>
void evaluate_time_dependent_operation_on_TimeIndexNode(Node* timeIndex)
{
    ParameterNode paramNode("my-param", VariabilityType::VARYING_IN_TIME_ONLY);
    LiteralNode literal(2.0);
    BinaryNode binary_node(&literal, &paramNode); // Correctly use the type as a template argument

    TimeIndexNode root(&binary_node, timeIndex);
    const std::string value = "dummy";

    MockLinearProblemData dummy_data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;
    std::vector<unsigned int> hours = {1, 2};

    Model model = createModelWithParameters(
      {Parameter("my-param", TimeDependent::YES, ScenarioDependent::NO)});
    auto param = build_context_parameter_with("my-param",
                                              value,
                                              VariabilityType::VARYING_IN_TIME_ONLY);
    const auto compoName = "1245";

    const std::vector<Component> components{createComponent(model, compoName, {param})};

    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    MockLinearProblem linearProblem = MockLinearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);
    EvalVisitor visitor(optimContainer,
                        {hours.at(0), hours.at(1) /*two hours*/, hours.at(0), hours.at(1), 0},
                        components.back(),
                        &dummy_data,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));

    const auto eval = visitor.dispatch(&root).valueAsDouble();

    std::vector<double> result_before_timeIndex = {evalExpected<BinaryNode>(literal.value(),
                                                                            hours.at(0)),
                                                   evalExpected<BinaryNode>(literal.value(),
                                                                            hours.at(1))};
    const auto timeIndexInt = static_cast<int>(visitor.dispatch(timeIndex).valueAsDouble());
    BOOST_CHECK_EQUAL(eval, result_before_timeIndex.at(timeIndexInt));
}

// Define a list of types (not instances)
using BinaryOperators = boost::mpl::
  list<MultiplicationNode, SumNode, SubtractionNode, DivisionNode>;

// Parametrize the test with types
BOOST_AUTO_TEST_CASE_TEMPLATE(evaluate_time_dependent_operations, T, BinaryOperators)
{
    evaluate_time_dependent_operation<T>();
} // Parametrize the test with types

BOOST_AUTO_TEST_CASE_TEMPLATE(evaluate_time_dependent_operations_time_shift_node,
                              T,
                              BinaryOperators)
{
    LiteralNode literal_node(-2);
    evaluate_time_dependent_operation_on_TimeShiftNode<T>(&literal_node);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(evaluate_time_dependent_operations_time_index_node,
                              T,
                              BinaryOperators)
{
    LiteralNode literal_node(1);
    evaluate_time_dependent_operation_on_TimeIndexNode<T>(&literal_node);
}

BOOST_FIXTURE_TEST_CASE(print_multiplication_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* mult = create<MultiplicationNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(mult);

    BOOST_CHECK_EQUAL(printed, "(22.000000*8.000000)");
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(mult).valueAsDouble(), num1 * num2);
}

BOOST_FIXTURE_TEST_CASE(print_division_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* div = create<DivisionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(div);

    BOOST_CHECK_EQUAL(printed, "(22.000000/8.000000)");
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(div).valueAsDouble(), num1 / num2);
}

BOOST_FIXTURE_TEST_CASE(print_division_by_zero, MyDummyFixture)
{
    double num1 = 22.0, num2 = 0.;
    Node* div = create<DivisionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(div);

    BOOST_CHECK_EQUAL(printed, "(22.000000/0.000000)");

    BOOST_CHECK_THROW(defaultComponentEvalVisitor->dispatch(div).valueAsDouble(),
                      EvalVisitorDivisionException);
}

BOOST_FIXTURE_TEST_CASE(DivisionNodeFull, MyDummyFixture)
{
    LiteralNode literalNode1(23.);
    LiteralNode literalNode2(-23.);

    DivisionNode divisionNode1(&literalNode1, &literalNode1);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(&divisionNode1).valueAsDouble(), 1.0);

    DivisionNode divisionNode2(&literalNode1, &literalNode2);
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(&divisionNode2).valueAsDouble(), -1.0);

    LiteralNode* literalNull = nullptr;

    DivisionNode divisionNode3(&literalNode1, literalNull);

    BOOST_CHECK_THROW(defaultComponentEvalVisitor->dispatch(&divisionNode3).valueAsDouble(),
                      InvalidNode);

    // truncated to zero
    LiteralNode literalVerySmall(1.e-323);

    DivisionNode divisionNode4(&literalNode1, &literalVerySmall);

    BOOST_CHECK_THROW(defaultComponentEvalVisitor->dispatch(&divisionNode4).valueAsDouble(),
                      EvalVisitorDivisionException);
}

BOOST_FIXTURE_TEST_CASE(print_subtraction_node, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* sub = create<SubtractionNode>(create<LiteralNode>(num1), create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(sub);

    BOOST_CHECK_EQUAL(printed, "(22.000000-8.000000)");
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(sub).valueAsDouble(), num1 - num2);
}

BOOST_FIXTURE_TEST_CASE(print_functionNode_max, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* max = create<FunctionNode>(FunctionNodeType::max,
                                     create<LiteralNode>(num1),
                                     create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(max);

    BOOST_CHECK_EQUAL(printed, "max(22.000000, 8.000000)");
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(max).valueAsDouble(), num1);
}

BOOST_FIXTURE_TEST_CASE(functionNode_max_timeDepdentParameter, TimeDependentParameterFixture)
{
    LiteralNode num1(22.0);
    LiteralNode num2(8);
    auto max = FunctionNode(FunctionNodeType::max, &num1, &num2, &paramNode);

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(&max);

    BOOST_CHECK_EQUAL(printed, "max(22.000000, 8.000000, my-param)");
    const auto& values = evalVisitor->dispatch(&max).valuesAsVector();
    BOOST_CHECK_EQUAL(values.size(), 2 /*two timesteps*/);
    BOOST_CHECK_EQUAL(values[0], 22.0);
}

BOOST_FIXTURE_TEST_CASE(functionNode_min, MyDummyFixture)
{
    double num1 = 22.0, num2 = 8;
    Node* min = create<FunctionNode>(FunctionNodeType::min,
                                     create<LiteralNode>(num1),
                                     create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(min);

    BOOST_CHECK_EQUAL(printed, "min(22.000000, 8.000000)");
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(min).valueAsDouble(), num2);
}

BOOST_AUTO_TEST_CASE(functionNode_min_timeDepdentParameter)
{
    TimeDependentParameterFixture fixture(
      {build_context_parameter_with("Param2", "P2", VariabilityType::VARYING_IN_TIME_ONLY)});
    fixture.dummy_data.addParams(
      std::make_pair<std::string, std::vector<double>>("P2", {-400, 1568}));
    ParameterNode second("Param2", VariabilityType::VARYING_IN_TIME_ONLY);
    auto min = FunctionNode(FunctionNodeType::min, &fixture.paramNode, &second);

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(&min);

    BOOST_CHECK_EQUAL(printed, "min(my-param, Param2)");
    const auto& values = fixture.evalVisitor->dispatch(&min).valuesAsVector();
    BOOST_CHECK_EQUAL(values.size(), 2 /*two timesteps*/);
    BOOST_CHECK_EQUAL(values[0], -400); // min(0, -400)
    BOOST_CHECK_EQUAL(values[1], 1);    // min(1, 1568)
}

BOOST_FIXTURE_TEST_CASE(functionNode_pow, MyDummyFixture)
{
    double num1 = 22.0, num2 = 2;
    Node* pow = create<FunctionNode>(FunctionNodeType::pow,
                                     create<LiteralNode>(num1),
                                     create<LiteralNode>(num2));

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(pow);

    BOOST_CHECK_EQUAL(printed, "pow(22.000000, 2.000000)");
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->dispatch(pow).valueAsDouble(),
                      std::pow(num1, num2));
}

BOOST_FIXTURE_TEST_CASE(functionNode_pow_timeDepdentParameter, TimeDependentParameterFixture)
{
    LiteralNode num2(2);
    auto pow = FunctionNode(FunctionNodeType::pow, &paramNode, &num2);

    PrintVisitor printVisitor;
    const auto printed = printVisitor.dispatch(&pow);

    BOOST_CHECK_EQUAL(printed, "pow(my-param, 2.000000)");
    BOOST_CHECK_EQUAL(evalVisitor->dispatch(&pow).value(0), std::pow(0, 2));
    BOOST_CHECK_EQUAL(evalVisitor->dispatch(&pow).value(1), std::pow(1, 2));
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
    BOOST_CHECK_THROW(defaultComponentEvalVisitor->dispatch(null_node).valueAsDouble(),
                      InvalidNode);
}

BOOST_AUTO_TEST_CASE(PrintVisitor_name)
{
    PrintVisitor printVisitor;
    BOOST_CHECK_EQUAL(printVisitor.name(), "PrintVisitor");
}

BOOST_FIXTURE_TEST_CASE(EvalVisitor_name, MyDummyFixture)
{
    BOOST_CHECK_EQUAL(defaultComponentEvalVisitor->name(), "EvalVisitor");
}

BOOST_FIXTURE_TEST_CASE(PrintTimeIndexNodeLiteralAndParameter, MyDummyFixture)
{
    Node* literal1 = create<LiteralNode>(1.);
    Node* param1 = create<ParameterNode>("value");
    Node* expr1 = create<TimeIndexNode>(literal1, param1);

    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(expr1) == "1.000000[ value ]");

    Node* expr2 = create<TimeIndexNode>(param1, literal1);
    BOOST_CHECK(printVisitor.dispatch(expr2) == "value[ 1.000000 ]");
}

BOOST_FIXTURE_TEST_CASE(PrintTimeIndexNodeArithmeticNodes, MyDummyFixture)
{
    Node* literal1 = create<LiteralNode>(1.);
    Node* param1 = create<ParameterNode>("value");
    Node* sum1 = create<SumNode>(literal1, param1);
    Node* sub1 = create<SubtractionNode>(param1, literal1);
    const Node* expr1 = create<TimeIndexNode>(sum1, sub1);

    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(expr1) == "(1.000000+value)[ (value-1.000000) ]");

    Node* mult1 = create<MultiplicationNode>(literal1, param1);
    Node* div1 = create<DivisionNode>(param1, literal1);
    const Node* expr2 = create<TimeIndexNode>(mult1, div1);
    BOOST_CHECK(printVisitor.dispatch(expr2) == "(1.000000*value)[ (value/1.000000) ]");
}

BOOST_FIXTURE_TEST_CASE(PrintTimeShiftNode, MyDummyFixture)
{
    Node* literal1 = create<LiteralNode>(1.);
    Node* literal2 = create<LiteralNode>(23);
    PrintVisitor printVisitor;
    // --
    Node* positive_shift = create<TimeShiftNode>(literal1, literal2);
    BOOST_CHECK(printVisitor.dispatch(positive_shift) == "1.000000[ t +23.000000 ]");
    // --

    Node* literal3 = create<LiteralNode>(-31);
    Node* negative_shift = create<TimeShiftNode>(literal1, literal3);

    BOOST_CHECK(printVisitor.dispatch(negative_shift) == "1.000000[ t -31.000000 ]");
}

BOOST_FIXTURE_TEST_CASE(PrintTimeSumNode, MyDummyFixture)
{
    Node* from = create<LiteralNode>(1.);
    Node* to = create<LiteralNode>(23);
    Node* expression = create<ParameterNode>("p");
    PrintVisitor printVisitor;
    // --
    Node* positive_shift = create<TimeSumNode>(from, to, expression);
    auto n = printVisitor.dispatch(positive_shift);
    BOOST_CHECK(printVisitor.dispatch(positive_shift) == "sum(t+1.000000 .. t+23.000000, p)");
    // --

    Node* mfrom = create<LiteralNode>(-1.);
    Node* mto = create<LiteralNode>(-23);
    Node* negative_shift = create<TimeSumNode>(mfrom, mto, expression);
    auto m = printVisitor.dispatch(negative_shift);

    BOOST_CHECK(printVisitor.dispatch(negative_shift) == "sum(t-1.000000 .. t-23.000000, p)");
}

BOOST_FIXTURE_TEST_CASE(PrintAllTimeSumNode, MyDummyFixture)
{
    Node* num = create<LiteralNode>(1.);
    Node* p = create<ParameterNode>("p");
    Node* expression = create<MultiplicationNode>(p, num);
    PrintVisitor printVisitor;
    // --
    Node* alltimesum = create<AllTimeSumNode>(expression);
    BOOST_CHECK(printVisitor.dispatch(alltimesum) == "sum((p*1.000000))");
    // --
}

BOOST_FIXTURE_TEST_CASE(PrintDualNode, MyDummyFixture)
{
    Node* dual = create<FunctionNode>(FunctionNodeType::dual,
                                      create<ParameterNode>("constraint"),
                                      create<LiteralNode>(0));
    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(dual) == "dual(constraint)[0.000000]");
}

BOOST_FIXTURE_TEST_CASE(PrintReducedCostNode, MyDummyFixture)
{
    Node* reducedCost = create<FunctionNode>(FunctionNodeType::reduced_cost,
                                             create<VariableNode>("var", 0));
    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(reducedCost) == "reduced_cost(var)");
}

BOOST_FIXTURE_TEST_CASE(print_floor_applied_to_a_literal, MyDummyFixture)
{
    Node* floor_node = create<FunctionNode>(FunctionNodeType::floor, create<LiteralNode>(2.3));
    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(floor_node) == "floor(2.300000)");
}

BOOST_FIXTURE_TEST_CASE(print_floor_applied_to_a_parameter, MyDummyFixture)
{
    Node* floor_node = create<FunctionNode>(FunctionNodeType::floor, create<ParameterNode>("p"));
    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(floor_node) == "floor(p)");
}

BOOST_FIXTURE_TEST_CASE(print_ceil_applied_to_a_literal, MyDummyFixture)
{
    Node* ceil_node = create<FunctionNode>(FunctionNodeType::ceil, create<LiteralNode>(3.7));
    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(ceil_node) == "ceil(3.700000)");
}

BOOST_FIXTURE_TEST_CASE(print_ceil_applied_to_a_parameter, MyDummyFixture)
{
    Node* ceil_node = create<FunctionNode>(FunctionNodeType::ceil, create<ParameterNode>("p"));
    PrintVisitor printVisitor;
    BOOST_CHECK(printVisitor.dispatch(ceil_node) == "ceil(p)");
}

BOOST_AUTO_TEST_CASE(testShiftEmptyVector)
{
    std::vector<int> emptyVector;
    std::vector<int> result = shiftVector(emptyVector, 5);
    BOOST_CHECK(result.empty());
}

BOOST_AUTO_TEST_CASE(testZeroShift)
{
    std::vector<int> zeroShiftVector = {1, 2, 3, 4, 5};
    std::vector<int> result = shiftVector(zeroShiftVector, 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                  result.end(),
                                  zeroShiftVector.begin(),
                                  zeroShiftVector.end());
}

BOOST_AUTO_TEST_CASE(testPositiveShift)
{
    std::vector<int> positiveShiftVector = {1, 2, 3, 4, 5};
    std::vector<int> expected = {3, 4, 5, 1, 2};
    std::vector<int> result = shiftVector(positiveShiftVector, 2);
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(testNegativeShift)
{
    std::vector<int> negativeShiftVector = {1, 2, 3, 4, 5};
    std::vector<int> expected = {4, 5, 1, 2, 3};
    std::vector<int> result = shiftVector(negativeShiftVector, -2);
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(testShiftEqualToSize)
{
    std::vector<int> equalShiftVector = {1, 2, 3, 4, 5};
    std::vector<int> result = shiftVector(equalShiftVector, 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                  result.end(),
                                  equalShiftVector.begin(),
                                  equalShiftVector.end());
}

BOOST_AUTO_TEST_CASE(testShiftGreaterThanSize)
{
    std::vector<int> greaterShiftVector = {1, 2, 3, 4, 5};
    std::vector<int> expected = {3, 4, 5, 1, 2};
    std::vector<int> result = shiftVector(greaterShiftVector, 7);
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(testSingleElementVector)
{
    std::vector<int> singleElementVector = {42};
    std::vector<int> result = shiftVector(singleElementVector, 3);
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                  result.end(),
                                  singleElementVector.begin(),
                                  singleElementVector.end());
}

BOOST_AUTO_TEST_CASE(testLargeShiftValues)
{
    std::vector<int> largeShiftVector = {1, 2, 3, 4, 5};
    std::vector<int> expectedPositive = {4, 5, 1, 2, 3};
    std::vector<int> expectedNegative = {3, 4, 5, 1, 2};
    // 1000003 % 5 = 3.
    std::vector<int> resultPositive = shiftVector(largeShiftVector, 1000003);
    // -1000003 % 5 = -3 and (-3 + 5) % 5 = 2
    std::vector<int> resultNegative = shiftVector(largeShiftVector, -1000003);
    BOOST_CHECK_EQUAL_COLLECTIONS(resultPositive.begin(),
                                  resultPositive.end(),
                                  expectedPositive.begin(),
                                  expectedPositive.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(resultNegative.begin(),
                                  resultNegative.end(),
                                  expectedNegative.begin(),
                                  expectedNegative.end());
}

BOOST_AUTO_TEST_CASE(TrimLeadingWhitespace)
{
    BOOST_CHECK_EQUAL(trimAndFormat("   example"), "+example");
}

BOOST_AUTO_TEST_CASE(PreserveLeadingMinus)
{
    BOOST_CHECK_EQUAL(trimAndFormat("   -value"), "-value");
}

BOOST_AUTO_TEST_CASE(PreserveLeadingPlus)
{
    BOOST_CHECK_EQUAL(trimAndFormat("   +text"), "+text");
}

BOOST_AUTO_TEST_CASE(AddPlusIfNoSign)
{
    BOOST_CHECK_EQUAL(trimAndFormat("noSign"), "+noSign");
}

BOOST_AUTO_TEST_CASE(HandleAlreadySignedString)
{
    BOOST_CHECK_EQUAL(trimAndFormat("-already"), "-already");
    BOOST_CHECK_EQUAL(trimAndFormat("+already"), "+already");
}

BOOST_AUTO_TEST_CASE(HandleOnlySpaces)
{
    BOOST_CHECK_EQUAL(trimAndFormat("   "), "");
}

BOOST_AUTO_TEST_CASE(HandleEmptyString)
{
    BOOST_CHECK_EQUAL(trimAndFormat(""), "");
}

BOOST_FIXTURE_TEST_CASE(testVariableNodeEvaluation, MyDummyFixture)
{
    Antares::Optimisation::LinearProblemApi::FillContext fillContext{0, 2, 10, 12, 0};
    ModelBuilder modelBuilder;

    unsigned varIndex = 0;
    auto variableBuilder = [](const std::string& id, TimeDependent timeDependent)
    {
        auto dummyExpression = []() { return Expression("", {}); };
        return Variable(id,
                        dummyExpression(),
                        dummyExpression(),
                        ValueType::FLOAT,
                        timeDependent,
                        ScenarioDependent::NO);
    };
    std::vector<Variable> variables;
    variables.reserve(2);
    variables.push_back(variableBuilder("my_const_variable", TimeDependent::NO));
    variables.push_back(variableBuilder("my_non_const_variable", TimeDependent::YES));

    auto model = modelBuilder.withVariables(std::move(variables)).build();

    ComponentBuilder component_builder;
    std::vector components = {component_builder.withModel(&model).withId("my_component").build()};
    LinearProblemDataImpl::LinearProblemData testData;

    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepo = makeScenarioGroupRepo(
      components.back());
    PredfinedSolutionLinearProblemMock linearProblem(true);
    OptimEntityContainer optimContainer(linearProblem);
    optimContainer.addFromSystemComponents(components);

    optimContainer.addStartColumn();
    linearProblem.addVariableValue(12.5); // my_const_variable
    std::vector<double> timeDepentVariableValues = {45.3, 78.9, 714.5};

    optimContainer.addStartColumn();
    for (const auto& value: timeDepentVariableValues)
    {
        linearProblem.addVariableValue(value);
    }

    Node* root = create<VariableNode>("my_const_variable",
                                      0,
                                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    EvalVisitor visitor(optimContainer,
                        fillContext,
                        components.back(),
                        &testData,
                        &scenarioGroupRepo.scenario(components.back().getScenarioGroupId()));
    double eval = visitor.dispatch(root).valueAsDouble();
    BOOST_CHECK_EQUAL(eval, 12.5);

    Node* reducedCost = create<FunctionNode>(
      FunctionNodeType::reduced_cost,
      create<VariableNode>("my_const_variable", 0, VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO));
    eval = visitor.dispatch(reducedCost).valueAsDouble();
    BOOST_CHECK_EQUAL(eval, 4.96);

    root = create<VariableNode>("my_const_variable", 0, VariabilityType::VARYING_IN_SCENARIO_ONLY);
    eval = visitor.dispatch(root).valueAsDouble();
    BOOST_CHECK_EQUAL(eval, 12.5);

    root = create<VariableNode>("my_non_const_variable", 1, VariabilityType::VARYING_IN_TIME_ONLY);
    auto evalVector = visitor.dispatch(root).valuesAsVector();
    BOOST_CHECK_EQUAL(evalVector.size(), 3);
    BOOST_CHECK_EQUAL(evalVector[0], 45.3);
    BOOST_CHECK_EQUAL(evalVector[1], 78.9);
    BOOST_CHECK_EQUAL(evalVector[2], 714.5);

    root = create<VariableNode>("my_non_const_variable",
                                1,
                                VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    evalVector = visitor.dispatch(root).valuesAsVector();
    BOOST_CHECK_EQUAL(evalVector.size(), 3);
    BOOST_CHECK_EQUAL(evalVector[0], 45.3);
    BOOST_CHECK_EQUAL(evalVector[1], 78.9);
    BOOST_CHECK_EQUAL(evalVector[2], 714.5);
}

BOOST_AUTO_TEST_SUITE_END()
