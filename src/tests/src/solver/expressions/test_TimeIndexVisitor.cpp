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
#include <antares/solver/expressions/visitors/PrintVisitor.h>
#include <antares/solver/expressions/visitors/TimeIndexVisitor.h>

using namespace Antares::Solver;
using namespace Antares::Solver::Nodes;
using namespace Antares::Solver::Visitors;

namespace bdata = boost::unit_test::data;

namespace Antares::Solver::Visitors
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
} // namespace Antares::Solver::Visitors
BOOST_AUTO_TEST_SUITE(_TimeIndexVisitor_)

BOOST_FIXTURE_TEST_CASE(simple_time_dependant_expression, Registry<Node>)
{
    PrintVisitor printVisitor;
    // LiteralNode --> constant in time and for all scenarios
    LiteralNode literalNode(65.);

    // Parameter --> constant in time and varying scenarios
    ParameterNode parameterNode1("p1", TimeIndex::VARYING_IN_SCENARIO_ONLY);

    // Variable time varying but constant across scenarios
    VariableNode variableNode1("v1", TimeIndex::VARYING_IN_TIME_ONLY);
    TimeIndexVisitor timeIndexVisitor;

    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&literalNode),
                      TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&parameterNode1),
                      TimeIndex::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(&variableNode1), TimeIndex::VARYING_IN_TIME_ONLY);

    // addition of literalNode, parameterNode1 and variableNode1 is time and scenario dependent
    Node* expr = create<SumNode>(&literalNode, &parameterNode1, &variableNode1);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(expr), TimeIndex::VARYING_IN_TIME_AND_SCENARIO);
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

BOOST_DATA_TEST_CASE_F(Registry<Node>,
                       simple_all,
                       bdata::make(TimeIndex_ALL) * bdata::make(operator_ALL),
                       timeIndex,
                       binaryOperator)
{
    auto [root, parameter] = binaryOperator(*this, timeIndex);
    TimeIndexVisitor timeIndexVisitor;
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(root), timeIndex);
    Node* neg = create<NegationNode>(root);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(neg), timeIndex);
}

template<class T>
static Node* singleNode(Registry<Node>& registry)
{
    return registry.create<T>("hello", "world");
}

static const std::vector<Node* (*)(Registry<Node>& registry)> singleNode_ALL{
  &singleNode<PortFieldNode>,
  &singleNode<PortFieldSumNode>,
  &singleNode<ComponentVariableNode>,
  &singleNode<ComponentParameterNode>};

BOOST_DATA_TEST_CASE_F(Registry<Node>,
                       signe_node,
                       bdata::make(TimeIndex_ALL) * bdata::make(singleNode_ALL),
                       timeIndex,
                       singleNode)
{
    Node* root = singleNode(*this);
    std::unordered_map<const Node*, TimeIndex> context;
    context[root] = timeIndex;
    TimeIndexVisitor timeIndexVisitor(context);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(root), timeIndex);
    Node* neg = create<NegationNode>(root);
    BOOST_CHECK_EQUAL(timeIndexVisitor.dispatch(neg), timeIndex);
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

BOOST_FIXTURE_TEST_CASE(TimeIndexVisitor_name, Registry<Node>)
{
    std::unordered_map<const Nodes::Node*, TimeIndex> context;
    TimeIndexVisitor timeIndexVisitor(context);
    BOOST_CHECK_EQUAL(timeIndexVisitor.name(), "TimeIndexVisitor");
}
BOOST_AUTO_TEST_SUITE_END()
