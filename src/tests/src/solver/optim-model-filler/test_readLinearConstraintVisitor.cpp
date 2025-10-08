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
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
using namespace Antares::Expressions;
using namespace Antares::ModelerStudy;
using namespace Antares::Expressions::Nodes;
using namespace Antares::Expressions::Visitors;

using namespace Antares::Optimisation;

BOOST_AUTO_TEST_SUITE(_read_linear_constraint_visitor_)

ScenarioGroupRepository createScenario()
{
    ScenarioGroupRepository scenarioGroupRepository;
    auto scenarioPtr = std::make_unique<LinearProblemDataImpl::Scenario>("SCENARIO_GROUP");
    scenarioPtr->setTimeSerieNumber(0, 1);
    scenarioGroupRepository.addScenario("SCENARIO_GROUP", std::move(scenarioPtr));
    scenarioPtr = std::make_unique<LinearProblemDataImpl::Scenario>("GROUP");
    scenarioPtr->setTimeSerieNumber(0, 1);
    scenarioGroupRepository.addScenario("GROUP", std::move(scenarioPtr));
    return scenarioGroupRepository;
}

struct MyDummyFixture: Registry<Node>
{
    LinearProblemMpsolverImpl::OrtoolsLinearProblem linearProblem; // TODO use mock
    LinearProblemDataImpl::LinearProblemData data;
    LinearProblemApi::EmptyScenario empty_scenario;
    ScenarioGroupRepository scenarioGroupRepository;
    SystemModel::Model m;
    SystemModel::ComponentBuilder componentBuilder;
    OptimEntityContainer optimContainer;
    SystemModel::Component component = componentBuilder.withId("compo")
                                         .withModel(&m)
                                         .withScenarioGroupId("group")
                                         .build();

    MyDummyFixture():
        linearProblem(false, "sirius"),
        scenarioGroupRepository(createScenario()),
        optimContainer(linearProblem, &data, &scenarioGroupRepository)
    {
        optimContainer.addFromSystemComponent(component);
        auto& optimComponent = optimContainer.getOptimComponent(0);
        optimComponent.index = 0;
        optimComponent.modelVariableGlobalIndices = {0, 1, 2};
        {
            optimContainer.addStartColumn();
            linearProblem.addNumVariable(0, 1, "var1");
            optimContainer.addStartColumn();
            linearProblem.addNumVariable(0, 1, "var2");
            optimContainer.addStartColumn();
            linearProblem.addNumVariable(0, 1, "var3");
        }
    }

    ReadLinearConstraintVisitor visitor()
    {
        LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};
        return ReadLinearConstraintVisitor(ctx, component, optimContainer);
    }

    void setComponentParameterValues(
      const std::vector<
        std::tuple<std::string, Antares::ModelerStudy::SystemModel::ParameterType, std::string>>&
        values)
    {
        std::map<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue> map;
        std::vector<SystemModel::Parameter> parameters;
        for (auto value: values)
        {
            map[std::get<0>(value)] = Antares::ModelerStudy::SystemModel::ParameterTypeAndValue{
              .id = std::get<0>(value),
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

std::pair<std::string, Antares::ModelerStudy::SystemModel::ParameterTypeAndValue>
build_context_parameter_with(
  const std::string& id,
  const std::string& value,
  const Antares::ModelerStudy::SystemModel::ParameterType& type = Antares::ModelerStudy::
    SystemModel::ParameterType::CONSTANT)
{
    return {id, {.id = id, .type = type, .value = value}};
}

BOOST_FIXTURE_TEST_CASE(test_visit_equal_node, MyDummyFixture)
{
    // 5 + var1 = var2 + 3 * var1 - param1(9)  ==> -2 * var1 - var2 = -14
    Node* lhs = create<SumNode>(create<LiteralNode>(5.), create<VariableNode>("var1", 0));
    Node* rhs = create<SumNode>(create<VariableNode>("var2", 1),
                                create<MultiplicationNode>(create<LiteralNode>(3.),
                                                           create<VariableNode>("var1", 0)),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<EqualNode>(lhs, rhs);
    setComponentParameterValues(
      {{"param1", Antares::ModelerStudy::SystemModel::ParameterType::CONSTANT, "9."}});
    const LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};

    auto v = visitor();
    auto constraint = v.dispatch(node);
    BOOST_CHECK_EQUAL(constraint.lb[0], -14.);
    BOOST_CHECK_EQUAL(constraint.ub[0], -14.);
    BOOST_REQUIRE_EQUAL(constraint.coef_per_var.size(), 1); // single timestep
    auto& linExpr = constraint.coef_per_var[0];
    BOOST_REQUIRE_EQUAL(linExpr.size(), 2); // two coeffs
    // var1
    BOOST_CHECK_EQUAL(linExpr[0].first, 0);
    BOOST_CHECK_EQUAL(linExpr[0].second, -2.);
    // var2
    BOOST_CHECK_EQUAL(linExpr[1].first, 1);
    BOOST_CHECK_EQUAL(linExpr[1].second, -1.);
}

BOOST_FIXTURE_TEST_CASE(test_visit_less_than_or_equal_node, MyDummyFixture)
{
    // -9 + var3 <= var1 + 5 * var2 - param1(10)  ==> - var1 - 5 * var2 + var3 <= -1
    Node* lhs = create<SumNode>(create<LiteralNode>(-9.), create<VariableNode>("var3", 2));
    Node* rhs = create<SumNode>(create<VariableNode>("var1", 0),
                                create<MultiplicationNode>(create<LiteralNode>(5.),
                                                           create<VariableNode>("var2", 1)),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<LessThanOrEqualNode>(lhs, rhs);
    setComponentParameterValues(
      {{"param1", Antares::ModelerStudy::SystemModel::ParameterType::CONSTANT, "10."}});
    const LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};

    auto v = visitor();
    auto constraint = v.dispatch(node);
    BOOST_CHECK_EQUAL(constraint.lb[0], -std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(constraint.ub[0], -1.);
    BOOST_REQUIRE_EQUAL(constraint.coef_per_var.size(), 1); // single timestep
    auto& linExpr = constraint.coef_per_var[0];
    BOOST_REQUIRE_EQUAL(linExpr.size(), 3); // 3 coeffs
    // var1
    BOOST_CHECK_EQUAL(linExpr[0].first, 0);
    BOOST_CHECK_EQUAL(linExpr[0].second, -1.);
    // var2
    BOOST_CHECK_EQUAL(linExpr[1].first, 1);
    BOOST_CHECK_EQUAL(linExpr[1].second, -5.);
    // var3
    BOOST_CHECK_EQUAL(linExpr[2].first, 2);
    BOOST_CHECK_EQUAL(linExpr[2].second, 1);
}

BOOST_FIXTURE_TEST_CASE(test_visit_greater_than_or_equal_node, MyDummyFixture)
{
    // 5 + var1 >= var2 + 3 * var1 - param1(9)  ==> -2 * var1 - var2 >= -14
    Node* lhs = create<SumNode>(create<LiteralNode>(5.), create<VariableNode>("var1", 0));
    Node* rhs = create<SumNode>(create<VariableNode>("var2", 1),
                                create<MultiplicationNode>(create<LiteralNode>(3.),
                                                           create<VariableNode>("var1", 0)),
                                create<NegationNode>(create<ParameterNode>("param1")));
    Node* node = create<GreaterThanOrEqualNode>(lhs, rhs);
    setComponentParameterValues(
      {{"param1", Antares::ModelerStudy::SystemModel::ParameterType::CONSTANT, "9."}});
    const LinearProblemApi::FillContext ctx{0, 0, 0, 0, 0};
    auto v = visitor();
    auto constraint = v.dispatch(node);
    BOOST_CHECK_EQUAL(constraint.lb[0], -14);
    BOOST_CHECK_EQUAL(constraint.ub[0], std::numeric_limits<double>::infinity());

    BOOST_REQUIRE_EQUAL(constraint.coef_per_var.size(), 1); // single timestep
    auto& linExpr = constraint.coef_per_var[0];
    BOOST_REQUIRE_EQUAL(linExpr.size(), 2); // 2 coeffs
    // var1
    BOOST_CHECK_EQUAL(linExpr[0].first, 0);
    BOOST_CHECK_EQUAL(linExpr[0].second, -2.);
    // var2
    BOOST_CHECK_EQUAL(linExpr[1].first, 1);
    BOOST_CHECK_EQUAL(linExpr[1].second, -1.);
}

BOOST_FIXTURE_TEST_CASE(test_visit_illegal_node, MyDummyFixture)
{
    auto lit = create<LiteralNode>(5.);
    std::vector<Node*> illegal_nodes = {create<SumNode>(),
                                        create<SubtractionNode>(lit, lit),
                                        create<MultiplicationNode>(lit, lit),
                                        create<DivisionNode>(lit, lit),
                                        create<NegationNode>(lit),
                                        create<VariableNode>("var", 99),
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
