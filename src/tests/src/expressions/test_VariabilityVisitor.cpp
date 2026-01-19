// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
          {build_context_parameter_with("param", "0", VariabilityType::VARYING_IN_SCENARIO_ONLY)},
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
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&parameterNode),
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&variableNode),
                      VariabilityType::VARYING_IN_TIME_ONLY);

    // addition of literalNode, parameterNode and variableNode is time and scenario dependent
    Node* expr = fixture.nodeRegistry.create<SumNode>(&literalNode, &parameterNode, &variableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(expr),
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(timeShift_expression)
{
    TimeShiftNode constant_time_shift_node(&literalNode, &parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&constant_time_shift_node),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeShiftNode scenario_only_time_shift_node(&parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&scenario_only_time_shift_node),
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);

    TimeShiftNode time_dep_only_time_shift_node(&variableNode, &literalNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&time_dep_only_time_shift_node),
                      VariabilityType::VARYING_IN_TIME_ONLY);
}

BOOST_AUTO_TEST_CASE(timeIndexNode_expression)
{
    TimeIndexNode t1(&literalNode, &parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t1),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeIndexNode t2(&parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t2),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeIndexNode t3(&variableNode, &literalNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t3),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(timeSumNode_expression)
{
    TimeSumNode t1(&literalNode, &parameterNode, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t1),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    TimeSumNode t2(&literalNode, &parameterNode, &parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t2), VariabilityType::VARYING_IN_SCENARIO_ONLY);

    TimeSumNode t3(&literalNode, &parameterNode, &variableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t3), VariabilityType::VARYING_IN_TIME_ONLY);
}

BOOST_AUTO_TEST_CASE(alltimeSumNode_expression)
{
    AllTimeSumNode t1(&literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t1),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    AllTimeSumNode t2(&parameterNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t2),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);

    AllTimeSumNode t3(&variableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&t3),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(variability_of_a_reduced_cost_operator___case_varying_in_time)
{
    VariableNode timeVariableNode{"my-var", 0, VariabilityType::VARYING_IN_TIME_ONLY};
    FunctionNode reducedCostNode(FunctionNodeType::reduced_cost, &timeVariableNode);
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&reducedCostNode),
                      VariabilityType::VARYING_IN_TIME_ONLY);
}

BOOST_AUTO_TEST_CASE(variability_of_reduced_cost_operator___case_constant)
{
    VariableNode timeVariableNode{"my-var", 0, VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO};
    FunctionNode reducedCostNode(FunctionNodeType::reduced_cost, &timeVariableNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&reducedCostNode),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(variability_of_the_dual_of_constraint___case_constant)
{
    LiteralNode zero(0);
    ParameterNode paramNode("param", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    FunctionNode dualNode(FunctionNodeType::dual, &zero, &paramNode);

    // gp : missing a constraint in the optimEntityContainer to test this case
    BOOST_CHECK(true); // avoiding warning about no check
    // BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&dualNode),
    //                   VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(variability_of_power_operator___case_varying_in_scenario_only)
{
    LiteralNode two(2);
    ParameterNode paramNode = ParameterNode("param", VariabilityType::VARYING_IN_TIME_ONLY);
    FunctionNode power(FunctionNodeType::pow, &paramNode, &two);

    // Because parameter varying status is set in fixture to VARYING_IN_SCENARIO_ONLY.
    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&power),
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
}

// gp : unit tests for max and min function are missing.

BOOST_AUTO_TEST_CASE(variability_of_floor_operator___case_constant)
{
    LiteralNode literalNode(2.3);
    FunctionNode floorNode(FunctionNodeType::floor, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&floorNode),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(variability_of_floor_operator___case_varying_in_scenario_only)
{
    ParameterNode paramNode = ParameterNode("param", VariabilityType::VARYING_IN_TIME_ONLY);
    FunctionNode floorNode(FunctionNodeType::floor, &paramNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&floorNode),
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
}

BOOST_AUTO_TEST_CASE(variability_of_ceil_operator___case_constant)
{
    LiteralNode literalNode(3.7);
    FunctionNode ceilNode(FunctionNodeType::ceil, &literalNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&ceilNode),
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(variability_of_ceil_operator___case_varying_in_scenario_only)
{
    ParameterNode paramNode = ParameterNode("param", VariabilityType::VARYING_IN_TIME_ONLY);
    FunctionNode ceilNode(FunctionNodeType::ceil, &paramNode);

    BOOST_CHECK_EQUAL(variabilityVisitor->dispatch(&ceilNode),
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
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
    BOOST_CHECK_EQUAL(VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | VariabilityType::VARYING_IN_TIME_ONLY,
                      VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO
                        | VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_ONLY
                        | VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_ONLY | VariabilityType::VARYING_IN_TIME_ONLY,
                      VariabilityType::VARYING_IN_TIME_ONLY);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_ONLY
                        | VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_ONLY
                        | VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | VariabilityType::VARYING_IN_TIME_ONLY,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      VariabilityType::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_SCENARIO_ONLY
                        | VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);

    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | VariabilityType::VARYING_IN_TIME_ONLY,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | VariabilityType::VARYING_IN_SCENARIO_ONLY,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK_EQUAL(VariabilityType::VARYING_IN_TIME_AND_SCENARIO
                        | VariabilityType::VARYING_IN_TIME_AND_SCENARIO,
                      VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_CASE(overwrite_variability_in_model_by_variablility_in_component)
{
    // When Optimisation::VariabilityType in component parameter is less varying than in model,
    // the
    // component's Optimisation::VariabilityType should be returned

    fixture.createModel("model",
                        {"param"},
                        {{"v1", ValueType::FLOAT, fixture.literal(0.), fixture.literal(100.)}},
                        {});
    fixture.createComponent(
      "model",
      "compo",
      {build_context_parameter_with("param", "0", VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO)},
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
                      VariabilityType::CONSTANT_IN_TIME_AND_SCENARIO);
}

BOOST_AUTO_TEST_SUITE_END()
