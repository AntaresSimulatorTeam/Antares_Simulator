// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <memory>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include <antares/study/system-model-base/variabilityType.h>
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include "UtilMocks.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::ModelerStudy::SystemModel;

// =================================
// Fixture to build AST nodes tree
// =================================
struct build_AST_fixture
{
    Node* literal(double value);
    Node* parameter(const std::string& name, const VariabilityType variability);
    Node* allTimeSum(Node* node);
    Node* timeSum(Node* from, Node* to, Node* p);
    Node* square(Node* node);

private:
    Registry<Nodes::Node> registry_;
};

Node* build_AST_fixture::literal(double value)
{
    return registry_.create<LiteralNode>(value);
}

Node* build_AST_fixture::allTimeSum(Node* node)
{
    return registry_.create<AllTimeSumNode>(node);
}

Node* build_AST_fixture::parameter(const std::string& name, const VariabilityType variability)
{
    return registry_.create<ParameterNode>(name, variability);
}

Node* build_AST_fixture::timeSum(Node* from, Node* to, Node* p)
{
    return registry_.create<TimeSumNode>(from, to, p);
}

Node* build_AST_fixture::square(Node* node)
{
    Node* exponant = literal(2);
    return registry_.create<FunctionNode>(FunctionNodeType::pow, node, exponant);
}

// =============================
// Fixture to build EvalVisitor
// =============================
struct build_eval_visitor_fixture
{
    build_eval_visitor_fixture();

    std::unique_ptr<Visitors::EvalVisitor> evalVisitor;

private:
    LinearProblemDataImpl::LinearProblemData data_;
    MockLinearProblem linearProblem_;
    std::unique_ptr<OptimEntityContainer> optimEntityContainer_;
    LinearProblemApi::FillContext fillCtx_;
    Model model_;
    Component component_;
    std::vector<Component> components_;
    ScenarioGroupRepository scenarioGroupRepo_;
};

build_eval_visitor_fixture::build_eval_visitor_fixture():
    linearProblem_(true),
    fillCtx_(0, 2, 0, 2, 0),
    model_(
      createModelWithParameters({Parameter("p", TimeDependent::YES, ScenarioDependent::NO),
                                 Parameter("five", TimeDependent::NO, ScenarioDependent::NO)})),
    component_(
      createComponent(model_,
                      "component-id",
                      {{"p", {"p", VariabilityType::VARYING_IN_TIME_ONLY, "p"}},
                       {"five", {"five", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO, "5"}}})),
    components_({component_}),
    scenarioGroupRepo_(makeScenarioGroupRepo(component_))
{
    // Parameter p : make assocaited time-series
    auto ts = std::make_unique<TimeSeriesSet>("p", 3);
    ts->add({1., 2., 3.});
    data_.addDataSeries(std::move(ts));

    // Creation of a OptimEntityContainer
    optimEntityContainer_ = std::make_unique<OptimEntityContainer>(linearProblem_);
    optimEntityContainer_->addFromSystemComponents(components_);

    // And finally, creation of the evaluation visitor (purpose of this fixture)
    const auto& scenario = scenarioGroupRepo_.scenario(component_.getScenarioGroupId());
    evalVisitor = std::make_unique<EvalVisitor>(*optimEntityContainer_,
                                                fillCtx_,
                                                component_,
                                                &data_,
                                                scenario);
}

// =================================================
// Test fixture : made from both previous fixtures
// =================================================
struct tests_fixture: build_AST_fixture, build_eval_visitor_fixture
{
};

BOOST_AUTO_TEST_SUITE(tests_on_sum_over_all_time_span)

BOOST_FIXTURE_TEST_CASE(sum_a_literal_over_time_span, tests_fixture)
{
    // Expression : sum(1.)
    Node* one = literal(1.);
    Node* sumOfOnes = allTimeSum(one);

    auto evalResult = evalVisitor->dispatch(sumOfOnes);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 3.);
}

BOOST_FIXTURE_TEST_CASE(sum_on_a_literal_over_time_span__then_square, tests_fixture)
{
    // Expression : sum(1.)^2
    Node* one = literal(1.);
    Node* sumOfOnes = allTimeSum(one);
    Node* squareSum = square(sumOfOnes);

    auto evalResult = evalVisitor->dispatch(squareSum);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 9.);
}

BOOST_FIXTURE_TEST_CASE(sum_a_parameter_as_time_series_over_time_span, tests_fixture)
{
    // Expression : sum(p), where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);
    Node* sum = allTimeSum(p);

    auto evalResult = evalVisitor->dispatch(sum);

    // Expected evaluation result : p1 + p2 + p3 = 6.
    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 6.);
}

BOOST_FIXTURE_TEST_CASE(sum_a_squared_param_as_time_series_over_time_span, tests_fixture)
{
    // Expression : sum(p^2), where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);
    Node* sum_of_squares = allTimeSum(square(p));

    auto evalResult = evalVisitor->dispatch(sum_of_squares);

    // Expected evaluation result : p1^2 + p2^2 + p3^2 = 1 + 4 + 9 = 14
    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 14.);
}

BOOST_FIXTURE_TEST_CASE(sum_a_parameter_as_time_series_over_time_span__then_square, tests_fixture)
{
    // Expression : sum(p)^2, where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);
    Node* sum = allTimeSum(p);
    Node* squareSum = square(sum);

    auto evalResult = evalVisitor->dispatch(squareSum);

    // Expected evaluation result : (p1 + p2 + p3)^2 = 36.
    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 36.);
}

BOOST_AUTO_TEST_SUITE_END()

// ==================================================================
// CAUTION :
// Suppose p is a parameter (either a literal, a constant parameter or a time series).
// In a yaml file, if sum operator is used with operator .., then it's a time sum, and
// it's always used with character 't' to indicate that a sum it made for each time step.
// For example : sum(t .. t+1, p).
//
// As a consequence,
// - the following expression : sum(0 .. 1, p) is rejected.
// - sum(t .. t+1, p) means that for each time step t, we sum p over the interval [t, t+1].
// - sum(t .. t+1, p) results in a vector sized to the number of hours, even if p is a literal
//   (that is of dimension 1).
//
// Please see following tests to be aware of sum(t .. t+1, p) properties.
// In code, time sum operator is represented by type TimeSumNode.
// ==================================================================
BOOST_AUTO_TEST_SUITE(tests_on_sum_over_interval_t__t_plus_1)

BOOST_FIXTURE_TEST_CASE(sum_a_parameter_as_time_series_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p), where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* sum = timeSum(from, to, p);

    auto evalResult = evalVisitor->dispatch(sum);

    // Expected evaluation result : (p1 + p2, p2 + p3, p3 + p1) = (3., 5., 4.).
    std::vector<double> expected = {3., 5., 4.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_squared_param_as_TS_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p^2), where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* sum_of_squares = timeSum(from, to, square(p));

    auto evalResult = evalVisitor->dispatch(sum_of_squares);

    // Expected evaluation result : (p1^2 + p2^2, p2^2 + p3^2, p3^2 + p1^2)
    // = (1^2 + 2^2, 2^2 + 3^2, 3^2 + 1^2) = (5., 13., 10.)
    std::vector<double> expected = {5., 13., 10.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_param_as_TS_on_interval_t__t_plus_1__then_square, tests_fixture)
{
    // Expression : sum(t .. t+1, p)^2, where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", VariabilityType::VARYING_IN_TIME_ONLY);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* squared_sum = square(timeSum(from, to, p));

    auto evalResult = evalVisitor->dispatch(squared_sum);

    // Expected evaluation result : ((p1 + p2)^2, (p2 + p3)^2, (p3 + p1)^2)
    // = ((1 + 2)^2, (2 + 3)^2, (3 + 1)^2) = (9., 25., 16.)
    std::vector<double> expected = {9., 25., 16.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_parameter_as_literal_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p), where p = 7
    Node* seven = literal(7);

    Node* from = literal(0); // for t + 0
    Node* to = literal(1);   // for t + 1
    Node* sum = timeSum(from, to, seven);

    auto evalResult = evalVisitor->dispatch(sum);

    // Expected evaluation result : (2p, 2p, 2p) = (14., 14., 14.)
    std::vector<double> expected = {14., 14., 14.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_param_as_literal_on_interval_t__t_plus_1__then_square, tests_fixture)
{
    // Expression : sum(t .. t+1, p)^2, where p = 7
    Node* seven = literal(7);

    Node* from = literal(0); // for t + 0
    Node* to = literal(1);   // for t + 1
    Node* squared_sum = square(timeSum(from, to, seven));

    auto evalResult = evalVisitor->dispatch(squared_sum);

    // Expected evaluation result : ((2p)^2, (2p)^2, (2p)^2) = (196, 196, 196)
    std::vector<double> expected = {196., 196., 196.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_squared_param_as_literal_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p^2), where p = 7
    Node* seven = literal(7);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* sum_of_squares = timeSum(from, to, square(seven));

    auto evalResult = evalVisitor->dispatch(sum_of_squares);

    // Expected evaluation result : (2(p^2), 2(p^2), 2(p^2)) = (98, 98, 98)
    std::vector<double> expected = {98., 98., 98.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_constant_parameter_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p), where p = 5
    Node* five = parameter("five", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* sum = timeSum(from, to, five);

    auto evalResult = evalVisitor->dispatch(sum);

    // Expected evaluation result : (2p, 2p, 2p) = (10., 10., 10.)
    std::vector<double> expected = {10., 10., 10.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_squared_constant_param_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p^2), where p = 5
    Node* five = parameter("five", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* sum_of_squares = timeSum(from, to, square(five));

    auto evalResult = evalVisitor->dispatch(sum_of_squares);

    // Expected evaluation result : (2*p^2, 2*p^2, 2*p^2) = (50., 50., 50.)
    std::vector<double> expected = {50., 50., 50.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_const_param_on_interval_t__t_plus_1__then_square, tests_fixture)
{
    // Expression : sum(t .. t+1, p)^2, where p = 5
    Node* five = parameter("five", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    Node* from = literal(0); // For t + 0
    Node* to = literal(1);   // For t + 1
    Node* squared_sum = square(timeSum(from, to, five));

    auto evalResult = evalVisitor->dispatch(squared_sum);

    // Expected evaluation result : ((2*p)^2, (2*p)^2, (2*p)^2) = (100., 100., 100.)
    std::vector<double> expected = {100., 100., 100.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_SUITE_END()
