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

#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/optim-model-filler/ReadLinearConstraintVisitor.h>
#include "antares/exception/InvalidArgumentError.hpp"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"

using namespace Antares::Expressions;
using namespace Antares::ModelerStudy;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

using namespace Antares::Optimization;

BOOST_AUTO_TEST_SUITE(_read_linear_constraint_visitor_)

struct MyDummyFixture: Registry<Node>
{
    Antares::Optimisation::LinearProblemDataImpl::LinearProblemData data;
    Antares::Optimisation::LinearProblemApi::EmptyScenario empty_scenario;
    Antares::Optimisation::ScenarioGroupRepository scenarioGroupRepository;
    SystemModel::Model m;
    SystemModel::ComponentBuilder componentBuilder;
    SystemModel::Component component = componentBuilder.withId("compo")
                                         .withModel(&m)
                                         .withScenarioGroupId("group")
                                         .build();

    MyDummyFixture()
    {
        auto scenarioPtr = std::make_unique<Antares::Optimisation::LinearProblemDataImpl::Scenario>(
          "SCENARIO_GROUP");
        scenarioPtr->setTimeSerieNumber(0, 1);
        scenarioGroupRepository.addScenario("SCENARIO_GROUP", std::move(scenarioPtr));
        scenarioPtr = std::make_unique<Antares::Optimisation::LinearProblemDataImpl::Scenario>(
          "GROUP");
        scenarioPtr->setTimeSerieNumber(0, 1);
        scenarioGroupRepository.addScenario("GROUP", std::move(scenarioPtr));
    }

    Antares::Optimisation::EvaluationContextProvider evaluationContextProvider()
    {
        return Antares::Optimisation::EvaluationContextProvider(data, scenarioGroupRepository);
    }

    ReadLinearConstraintVisitor visitor()
    {
        Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};
        return ReadLinearConstraintVisitor(evaluationContextProvider(), ctx, component);
    }

    void setComponentParameterValues(
      const std::vector<std::tuple<std::string, ParameterType, std::string>>& values)
    {
        std::map<std::string, ParameterTypeAndValue> map;
        std::vector<SystemModel::Parameter> parameters;
        for (auto value: values)
        {
            map[std::get<0>(value)] = ParameterTypeAndValue{.id = std::get<0>(value),
                                                            .type = std::get<1>(value),
                                                            .value = std::get<2>(value)};
            SystemModel::Parameter parameter{std::get<0>(value),
                                             SystemModel::TimeDependent::YES,
                                             SystemModel::ScenarioDependent::YES};
            parameters.push_back(parameter);
        }
        SystemModel::ModelBuilder modelBuilder;
        m = modelBuilder.withId("model").withParameters(std::move(parameters)).build();
        component = componentBuilder.withId("compo")
                      .withModel(&m)
                      .withScenarioGroupId("group")
                      .withParameterValues(map)
                      .build();
    }
};

BOOST_FIXTURE_TEST_CASE(test_name, MyDummyFixture)
{
    BOOST_CHECK_EQUAL(visitor().name(), "ReadLinearConstraintVisitor");
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
    setComponentParameterValues({{"param1", ParameterType::CONSTANT, "9."}});
    const Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};
    ReadLinearConstraintVisitor visitor(evaluationContextProvider(), ctx, component);
    auto constraint = visitor.dispatch(node)[0];
    BOOST_CHECK_EQUAL(constraint.lb, -14.);
    BOOST_CHECK_EQUAL(constraint.ub, -14.);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.size(), 2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var1", MCYearAndTime::MCYear{0}, 0)),
                      -2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var2", MCYearAndTime::MCYear{0}, 0)),
                      -1);
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
    setComponentParameterValues({{"param1", ParameterType::CONSTANT, "10."}});
    const Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};
    ReadLinearConstraintVisitor visitor(evaluationContextProvider(), ctx, component);
    auto constraint = visitor.dispatch(node)[0];
    BOOST_CHECK_EQUAL(constraint.lb, -std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(constraint.ub, -1.);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.size(), 3);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var1", MCYearAndTime::MCYear{0}, 0)),
                      -1);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var2", MCYearAndTime::MCYear{0}, 0)),
                      -5);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var3", MCYearAndTime::MCYear{0}, 0)),
                      1);
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
    setComponentParameterValues({{"param1", ParameterType::CONSTANT, "9."}});
    const Antares::Optimisation::LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};
    ReadLinearConstraintVisitor visitor(evaluationContextProvider(), ctx, component);
    auto constraint = visitor.dispatch(node)[0];
    BOOST_CHECK_EQUAL(constraint.lb, -14);
    BOOST_CHECK_EQUAL(constraint.ub, std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(constraint.coef_per_var.size(), 2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var1", MCYearAndTime::MCYear{0}, 0)),
                      -2);
    BOOST_CHECK_EQUAL(constraint.coef_per_var.at(
                        FullKey(component.Id(), "var2", MCYearAndTime::MCYear{0}, 0)),
                      -1);
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
                                        create<TimeShiftNode>(lit, lit),
                                        create<TimeIndexNode>(lit, lit),
                                        create<TimeSumNode>(lit, lit, lit),
                                        create<AllTimeSumNode>(lit)};

    for (Node* node: illegal_nodes)
    {
        BOOST_CHECK_EXCEPTION(visitor().dispatch(node),
                              Antares::Error::InvalidArgumentError,
                              checkMessage("Root node of a constraint must be a comparator."));
    }
}

BOOST_AUTO_TEST_SUITE_END()
