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

#include <inmemory-modeler.h>
#include <string>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/VariabilityVisitor.h>
#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include "antares/optimisation/linear-problem-api/IScenario.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"

#include "mockModelerObjects.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;
using namespace Antares::Optimisation;

namespace bdata = boost::unit_test::data;

namespace Antares::Optimisation
{
static std::ostream& operator<<(std::ostream& os, VariabilityType v)
{
    switch (v)
    {
    case VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO:
        return os << "Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO";
    case VariabilityType::VARYING_IN_TIME_ONLY:
        return os << "Optimisation::VariabilityType::VARYING_IN_TIME_ONLY";
    case VariabilityType::VARYING_IN_SCENARIO_ONLY:
        return os << "Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY";
    case VariabilityType::VARYING_IN_TIME_AND_SCENARIO:
        return os << "Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO";
    default:
        return os << "<unknown>";
    }
}
} // namespace Antares::Optimisation

class DefaultScenario final: public LinearProblemApi::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // Default rank for empty groupId
    }
};

struct TestVariabilityVisitorFixture
{
    Test::Modeler::LinearProblemBuildingFixture fixture;
    LiteralNode literalNode{65.};
    ParameterNode parameterNode{"param", VariabilityType::VARYING_IN_SCENARIO_ONLY};
    VariableNode variableNode{"v1", 0, VariabilityType::VARYING_IN_TIME_ONLY};
    std::optional<VariabilityVisitor> variabilityVisitor;

    void setup()
    {
        fixture.createModel("model",
                            {"param"},
                            {{"v1", ValueType::FLOAT, fixture.literal(0.), fixture.literal(100.)}},
                            {});
        fixture.createComponent(
          "model",
          "compo",
          {build_context_parameter_with("param", "0", ParameterType::TIMESERIES)},
          "group");

        auto bounds_time_series = std::make_unique<LinearProblemDataImpl::TimeSeriesSet>("bounds",
                                                                                         3);
        LinearProblemApi::FillContext ctx{0, 2, 0, 2, 0};
        // setting 3 hours (including h 1 and 2)
        bounds_time_series->add({10., 11., 12.});
        LinearProblemDataImpl::LinearProblemData data;
        data.addDataSeries(std::move(bounds_time_series));
        std::vector<std::unique_ptr<LinearProblemApi::IScenario>> scenarios;
        auto scenario0 = std::make_unique<LinearProblemDataImpl::Scenario>("group");
        scenario0->setTimeSerieNumber(0, 0);
        scenarios.push_back(std::move(scenario0));

        fixture.buildLinearProblem(ctx, data, scenarios);
        variabilityVisitor.emplace(*fixture.optimEntityContainer, fixture.components[0]);
    }
};

BOOST_FIXTURE_TEST_SUITE(TestVariabilityVisitor, TestVariabilityVisitorFixture)

BOOST_AUTO_TEST_CASE(simple_time_dependant_expression)
{
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&literalNode),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&parameterNode),
                      Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&variableNode),
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY);

    // addition of literalNode, parameterNode and variableNode is time and scenario dependent
    Node* expr = fixture.nodeRegistry.create<SumNode>(&literalNode, &parameterNode, &variableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(expr),
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(timeShift_expression)
{
    TimeShiftNode constant_time_shift_node(&literalNode, &parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&constant_time_shift_node),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeShiftNode scenario_only_time_shift_node(&parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&scenario_only_time_shift_node),
                      Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY);

    TimeShiftNode time_dep_only_time_shift_node(&variableNode, &literalNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&time_dep_only_time_shift_node),
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY);
}

BOOST_AUTO_TEST_CASE(timeIndexNode_expression)
{
    TimeIndexNode t1(&literalNode, &parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t1),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeIndexNode t2(&parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t2),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeIndexNode t3(&variableNode, &literalNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t3),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(timeSumNode_expression)
{
    TimeSumNode t1(&literalNode, &parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t1),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeSumNode t2(&literalNode, &parameterNode, &parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t2),
                      Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY);

    TimeSumNode t3(&literalNode, &parameterNode, &variableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t3),
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY);
}

BOOST_AUTO_TEST_CASE(alltimeSumNode_expression)
{
    AllTimeSumNode t1(&literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t1),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    AllTimeSumNode t2(&parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t2),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    AllTimeSumNode t3(&variableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t3),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

static const std::vector<VariabilityType> VariabilityType_ALL{
  VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
  VariabilityType::VARYING_IN_TIME_ONLY,
  VariabilityType::VARYING_IN_SCENARIO_ONLY,
  VariabilityType::VARYING_IN_TIME_AND_SCENARIO};

template<class T>
static std::pair<Node*, ParameterNode*> s_(Registry<Node>& registry,
                                           const VariabilityType& time_index)
{
    Node* left = registry.create<LiteralNode>(42.);
    ParameterNode* right = registry.create<ParameterNode>("param", time_index);
    return {registry.create<T>(left, right), right};
}

static const std::vector<std::pair<Node*, ParameterNode*> (*)(Registry<Node>&,
                                                              const VariabilityType&)>
  operator_ALL{&s_<SumNode>,
               &s_<SubtractionNode>,
               &s_<MultiplicationNode>,
               &s_<DivisionNode>,
               &s_<EqualNode>,
               &s_<LessThanOrEqualNode>,
               &s_<GreaterThanOrEqualNode>};

BOOST_DATA_TEST_CASE_F(TestVariabilityVisitorFixture,
                       simple_all,
                       bdata::make(VariabilityType_ALL) * bdata::make(operator_ALL),
                       variability,
                       binaryOperator)
{
    auto [root, parameter] = binaryOperator(fixture.nodeRegistry, variability);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(root), variability);

    Node* neg = fixture.nodeRegistry.create<NegationNode>(root);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(neg), variability);
}

template<class T>
static Node* singleNode(Registry<Node>& registry)
{
    return registry.create<T>("hello", "world");
}

static const std::vector<Node* (*)(Registry<Node>&)> nodesNotHandledByVariabilityVisitor{
  &singleNode<PortFieldNode>};

BOOST_DATA_TEST_CASE_F(TestVariabilityVisitorFixture,
                       catching_exceptions_when_visiting_not_handled_nodes,
                       bdata::make(nodesNotHandledByVariabilityVisitor),
                       not_handled_node)
{
    Node* nonHandldedNode = not_handled_node(fixture.nodeRegistry);
    BOOST_CHECK_THROW(variabilityVisitor->dispatch(nonHandldedNode), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_time_index_logical_operator)
{
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY
                        | Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY
                        | Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      Antares::Optimisation::VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(test_overwrite_time_inde_from_component)
{
    // When Optimisation::VariabilityType in component parameter is less varying than in model,
    // the
    // component's Optimisation::VariabilityType should be returned

    fixture.createModel("model",
                        {"param"},
                        {{"v1", ValueType::FLOAT, fixture.literal(0.), fixture.literal(100.)}},
                        {});
    fixture.createComponent("model",
                            "compo",
                            {build_context_parameter_with("param", "0", ParameterType::CONSTANT)},
                            "group");

    auto bounds_time_series = std::make_unique<LinearProblemDataImpl::TimeSeriesSet>("bounds", 3);
    LinearProblemApi::FillContext ctx{0, 2, 0, 2, 0};
    // setting 3 hours (including h 1 and 2)
    bounds_time_series->add({10., 11., 12.});
    LinearProblemDataImpl::LinearProblemData data;
    data.addDataSeries(std::move(bounds_time_series));
    std::vector<std::unique_ptr<LinearProblemApi::IScenario>> scenarios;
    auto scenario0 = std::make_unique<LinearProblemDataImpl::Scenario>("group2");
    scenario0->setTimeSerieNumber(0, 0);
    scenarios.push_back(std::move(scenario0));

    fixture.buildLinearProblem(ctx, data, scenarios);
    variabilityVisitor.emplace(*fixture.optimEntityContainer, fixture.components[1]);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&parameterNode),
                      Antares::Optimisation::VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_SUITE_END()
