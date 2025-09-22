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
#if 0
#define WIN32_LEAN_AND_MEAN

#include <string>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/PrintVisitor.h>
#include <antares/expressions/visitors/TimeIndexVisitor.h>
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"
#include "antares/optimisation/linear-problem-api/IScenario.h"

#include "mockModelerObjects.h"

using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;
using namespace Antares::ModelerStudy;

namespace bdata = boost::unit_test::data;

namespace Antares::Expressions::Visitors
{
static std::ostream& operator<<(std::ostream& os, TimeIndex s)
{
    switch (s)
    {
    case TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO:
        return os << "TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO";
    case TimeIndex::VARYING_IN_TIME_ONLY:
        return os << "TimeIndex::VARYING_IN_TIME_ONLY";
    case TimeIndex::VARYING_IN_SCENARIO_ONLY:
        return os << "TimeIndex::VARYING_IN_SCENARIO_ONLY";
    case TimeIndex::VARYING_IN_TIME_AND_SCENARIO:
        return os << "TimeIndex::VARYING_IN_TIME_AND_SCENARIO";
    default:
        return os << "<unknown>";
    }
}
} // namespace Antares::Expressions::Visitors
BOOST_AUTO_TEST_SUITE(_TimeIndexVisitor_)

struct BasicFixture: Registry<Node>
{
    // LiteralNode --> constant in time and for all scenarios
    LiteralNode literalNode{65.};
    ParameterNode parameterNode{"p1", TimeIndex::VARYING_IN_SCENARIO_ONLY};
    VariableNode variableNode{"v1", TimeIndex::VARYING_IN_TIME_ONLY};
    SystemModel::Model m;
    SystemModel::ComponentBuilder componentBuilder;
    const SystemModel::Component component = componentBuilder.withId("compo")
                                               .withModel(&m)
                                               .withScenarioGroupId("group")
                                               .build();

    // Mock dependency
    struct MockLinearProblemData: Antares::Optimisation::LinearProblemApi::ILinearProblemData
    {
        [[nodiscard]] double getData(const std::string&, unsigned int, unsigned int) const override
        {
            return 123.45; // Mock return value for testing
        }
    } mockData;

    std::map<std::string, ParameterTypeAndValue> system_parameters = {
      {"p1", {"p1", ParameterType::TIMESERIE, "42.5"}},
      {"param", {"param", ParameterType::TIMESERIE, "1234"}},
    };

    std::map<std::string, double> variables; // Not needed for this test

    Antares::Optimisation::LinearProblemApi::EmptyScenario emptyScenario;

    EvaluationContext context;
    MockEvaluationContextProvider contextProvider = MockEvaluationContextProvider(context);
    TimeIndexVisitor timeIndexVisitor;

    BasicFixture():
        context(system_parameters, variables, mockData, emptyScenario),
        timeIndexVisitor(component, contextProvider)
    {
    }
};

BOOST_FIXTURE_TEST_CASE(simple_time_dependant_expression, BasicFixture)
{
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&literalNode),
                      TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&parameterNode),
                      TimeIndex::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&variableNode), TimeIndex::VARYING_IN_TIME_ONLY);

    // addition of literalNode, parameterNode and variableNode is time and scenario dependent
    Node* expr = create<SumNode>(&literalNode, &parameterNode, &variableNode);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(expr), TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_FIXTURE_TEST_CASE(timeShift_expression, BasicFixture)
{
    TimeShiftNode constant_time_shift_node(&literalNode, &parameterNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&constant_time_shift_node),
                      TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeShiftNode scenario_only_time_shift_node(&parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&scenario_only_time_shift_node),
                      TimeIndex::VARYING_IN_SCENARIO_ONLY);

    TimeShiftNode time_dep_only_time_shift_node(&variableNode, &literalNode);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&time_dep_only_time_shift_node),
                      TimeIndex::VARYING_IN_TIME_ONLY);
}

BOOST_FIXTURE_TEST_CASE(timeIndexNode_expression, BasicFixture)
{
    TimeIndexNode t1(&literalNode, &parameterNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t1), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeIndexNode t2(&parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t2), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeIndexNode t3(&variableNode, &literalNode);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t3), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_FIXTURE_TEST_CASE(timeSumNode_expression, BasicFixture)
{
    TimeSumNode t1(&literalNode, &parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t1), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeSumNode t2(&literalNode, &parameterNode, &parameterNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t2), TimeIndex::VARYING_IN_SCENARIO_ONLY);

    TimeSumNode t3(&literalNode, &parameterNode, &variableNode);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t3), TimeIndex::VARYING_IN_TIME_ONLY);
}

BOOST_FIXTURE_TEST_CASE(alltimeSumNode_expression, BasicFixture)
{
    AllTimeSumNode t1(&literalNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t1), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    AllTimeSumNode t2(&parameterNode);

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t2), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    AllTimeSumNode t3(&variableNode);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&t3), TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
}

static const std::vector<TimeIndex> TimeIndex_ALL{TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                                                  TimeIndex::VARYING_IN_TIME_ONLY,
                                                  TimeIndex::VARYING_IN_SCENARIO_ONLY,
                                                  TimeIndex::VARYING_IN_TIME_AND_SCENARIO};

template<class T>
static std::pair<Node*, ParameterNode*> s_(Registry<Node>& registry, const TimeIndex& time_index)
{
    Node* left = registry.create<LiteralNode>(42.);
    ParameterNode* right = registry.create<ParameterNode>("param", time_index);
    return {registry.create<T>(left, right), right};
}

static const std::vector<std::pair<Node*, ParameterNode*> (*)(Registry<Node>& registry,
                                                              const TimeIndex& time_index)>
  operator_ALL{&s_<SumNode>,
               &s_<SubtractionNode>,
               &s_<MultiplicationNode>,
               &s_<DivisionNode>,
               &s_<EqualNode>,
               &s_<LessThanOrEqualNode>,
               &s_<GreaterThanOrEqualNode>};

BOOST_DATA_TEST_CASE_F(BasicFixture,
                       simple_all,
                       bdata::make(TimeIndex_ALL) * bdata::make(operator_ALL),
                       timeIndex,
                       binaryOperator)
{
    auto [root, parameter] = binaryOperator(*this, timeIndex);
    TimeIndexVisitor timeIndexVisitor(component, contextProvider);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(root), timeIndex);

    Node* neg = create<NegationNode>(root);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(neg), timeIndex);
}

template<class T>
static Node* singleNode(Registry<Node>& registry)
{
    return registry.create<T>("hello", "world");
}

static const std::vector<Node* (*)(Registry<Node>& registry)> nodesNotHandledByTimeIndexVisitor{
  &singleNode<PortFieldNode>};

BOOST_DATA_TEST_CASE_F(BasicFixture,
                       catching_exceptions_when_visiting_not_handled_nodes,
                       bdata::make(nodesNotHandledByTimeIndexVisitor),
                       not_handled_node)
{
    Node* nonHandldedNode = not_handled_node(*this);
    TimeIndexVisitor timeIndexVisitor(component, contextProvider);
    BOOST_CHECK_THROW(timeIndexVisitor.dispatch(nonHandldedNode), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_time_index_logical_operator)
{
    BOOST_CHECK_EQUAL(TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
                        | TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                      TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO | TimeIndex::VARYING_IN_TIME_ONLY,
                      TimeIndex::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
                        | TimeIndex::VARYING_IN_SCENARIO_ONLY,
                      TimeIndex::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO
                        | TimeIndex::VARYING_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_ONLY | TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_ONLY | TimeIndex::VARYING_IN_TIME_ONLY,
                      TimeIndex::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_ONLY | TimeIndex::VARYING_IN_SCENARIO_ONLY,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_ONLY | TimeIndex::VARYING_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_SCENARIO_ONLY
                        | TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_SCENARIO_ONLY | TimeIndex::VARYING_IN_TIME_ONLY,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_SCENARIO_ONLY | TimeIndex::VARYING_IN_SCENARIO_ONLY,
                      TimeIndex::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_SCENARIO_ONLY | TimeIndex::VARYING_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_AND_SCENARIO
                        | TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_AND_SCENARIO | TimeIndex::VARYING_IN_TIME_ONLY,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_AND_SCENARIO | TimeIndex::VARYING_IN_SCENARIO_ONLY,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(TimeIndex::VARYING_IN_TIME_AND_SCENARIO
                        | TimeIndex::VARYING_IN_TIME_AND_SCENARIO,
                      TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_FIXTURE_TEST_CASE(test_overwrite_time_inde_from_component, BasicFixture)
{
    // When TimeIndex in component parameter is less varying than in model, the component's
    // TimeIndex should be returned
    ParameterNode parameterNode{"p1", TimeIndex::VARYING_IN_TIME_AND_SCENARIO};

    EvaluationContext context1(
      {{"p1", ParameterTypeAndValue{.id = "p1", .type = ParameterType::CONSTANT, .value = "1"}}},
      {},
      mockData,
      emptyScenario);
    auto provider1 = MockEvaluationContextProvider(context1);
    TimeIndexVisitor timeIndexVisitor1{component, provider1};
    auto timeIndex = timeIndexVisitor1.dispatch(&parameterNode);
    BOOST_CHECK_EQUAL(timeIndex, TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);

    EvaluationContext context2(
      {{"p1", ParameterTypeAndValue{.id = "p1", .type = ParameterType::TIMESERIE, .value = "1"}}},
      {},
      mockData,
      emptyScenario);
    auto provider2 = MockEvaluationContextProvider(context2);
    TimeIndexVisitor timeIndexVisitor2{component, provider2};
    timeIndex = timeIndexVisitor2.dispatch(&parameterNode);
    BOOST_CHECK_EQUAL(timeIndex, TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_SUITE_END()
#endif
