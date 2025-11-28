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
 * along with Antares_Simulator. If not, see
 * <https://opensource.org/license/mpl-2-0/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <memory>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/EvalVisitor.h>
#include "antares/modeler-optimisation-container/TimeIndex.h"
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
    Node* parameter(const std::string& name, const TimeIndex variability);
    Node* allTimeSum(Node* node);
    Node* timeSum(Node* from, Node* to, Node* p);

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

Node* build_AST_fixture::parameter(const std::string& name, const TimeIndex variability)
{
    return registry_.create<ParameterNode>(name, variability);
}

Node* build_AST_fixture::timeSum(Node* from, Node* to, Node* p)
{
    return registry_.create<TimeSumNode>(from, to, p);
}

// =============================
// Fixture to build EvalVisitor
// =============================
struct build_eval_visitor_fixture
{
    build_eval_visitor_fixture();

    std::unique_ptr<Visitors::EvalVisitor> evaluator;

private:
    LinearProblemDataImpl::LinearProblemData data_;
    MockLinearProblem linearProblem_;
    OptimEntityContainer optimEntityContainer_;
    LinearProblemApi::FillContext fillCtx_;
    Model model_;
    Component component_;
    std::vector<Component> components_;
    ScenarioGroupRepository scenarioGroupRepository_;
};

build_eval_visitor_fixture::build_eval_visitor_fixture():
    linearProblem_(true),
    fillCtx_(0, 2, 0, 2, 0),
    model_(
      createModelWithParameters({Parameter("p", TimeDependent::YES, ScenarioDependent::NO),
                                 Parameter("five", TimeDependent::YES, ScenarioDependent::NO)})),
    component_(createComponent(model_,
                               "component",
                               {{"p", {"p", ParameterType::TIMESERIE, "p"}},
                                {"five", {"five", ParameterType::CONSTANT, "5"}}},
                               0)),
    scenarioGroupRepository_(getscenarioGroupRepository(component_)),
    components_({component_}),
    optimEntityContainer_(linearProblem_, &data_, &scenarioGroupRepository_)
{
    auto ts = std::make_unique<TimeSeriesSet>("p", 3);
    ts->add({1., 2., 3.});
    data_.addDataSeries(std::move(ts));

    optimEntityContainer_.addFromSystemComponents(components_);
    evaluator = std::make_unique<EvalVisitor>(optimEntityContainer_, fillCtx_, component_);
}

// =================================================
// Test fixture : made from both previous fixtures
// =================================================
struct tests_fixture: build_AST_fixture, build_eval_visitor_fixture
{
};

BOOST_FIXTURE_TEST_CASE(sum_a_literal_over_time_span, tests_fixture)
{
    // Expression : sum(1.)
    Node* one = literal(1.);
    Node* sumOfOnes = allTimeSum(one);

    auto evalResult = evaluator->dispatch(sumOfOnes);

    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 3.);
}

BOOST_FIXTURE_TEST_CASE(sum_a_parameter_over_time_span, tests_fixture)
{
    // Expression : sum(p), where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", TimeIndex::VARYING_IN_TIME_ONLY);
    Node* sum = allTimeSum(p);

    auto evalResult = evaluator->dispatch(sum);

    // Expected result of sum evaluation is p1 + p2 + p3 = 6.
    BOOST_CHECK_EQUAL(evalResult.valueAsDouble(), 6.);
}

BOOST_FIXTURE_TEST_CASE(sum_a_parameter_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p), where p = {p1, p2, p3} = {1., 2., 3.}
    Node* p = parameter("p", TimeIndex::VARYING_IN_TIME_ONLY);

    Node* from = literal(0);
    Node* to = literal(1);
    Node* sum = timeSum(from, to, p);

    auto evalResult = evaluator->dispatch(sum);

    // Expected result : (p1 + p2, p2 + p3, p3 + p1) = (3., 5., 4.).
    std::vector<double> expected = {3., 5., 4.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}

BOOST_FIXTURE_TEST_CASE(sum_a_literal_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p), where p = 7
    Node* seven = literal(7);

    Node* from = literal(0);
    Node* to = literal(1);
    Node* sum = timeSum(from, to, seven);

    auto evalResult = evaluator->dispatch(sum);

    // Expected result : 2p = 14
    double expected = 14;
    double actual = evalResult.valueAsDouble();
    BOOST_CHECK(actual == expected);
}

/*
BOOST_FIXTURE_TEST_CASE(sum_a_constant_parameter_on_interval_t__t_plus_1, tests_fixture)
{
    // Expression : sum(t .. t+1, p), where p = 5
    Node* five = parameter("five", TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    Node* from = literal(0.);
    Node* to = literal(1.);
    Node* sum = timeSum(from, to, five);

    auto evalResult = evaluator->dispatch(sum);

    // Expected result : (2p, 2p, 2p) = (10, 10., 10.).
    std::vector<double> expected = {10., 10., 10.};
    std::vector<double> actual = evalResult.valuesAsVector();
    BOOST_CHECK(actual == expected);
}
*/
