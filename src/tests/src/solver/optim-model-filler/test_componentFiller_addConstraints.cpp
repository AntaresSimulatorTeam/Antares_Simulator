// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/study/system-model-base/variabilityType.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-data-impl/timeSeriesSet.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"
#include "antares/study/system-model/parameter.h"
#include "antares/study/system-model/timeAndScenarioType.h"

#include "inmemory-modeler.h"
#include "unit_test_utils.h"

using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::Optimisation;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimization;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Test::Modeler;
using namespace std;

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addConstraints_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(ct_one_var__pb_contains_the_ct)
{
    // var1 <= 3
    auto var_node = variable("var1", 0);
    auto three = literal(3);
    auto ct_node = nodeRegistry.create<LessThanOrEqualNode>(var_node, three);

    createModel("model",
                {},
                {{"var1", ValueType::BOOL, literal(-5), literal(10), false, false}},
                {{"ct1", ct_node}});
    createComponent("model", "componentToto");
    buildLinearProblem();

    auto var = pb->lookupVariable("componentToto.var1");
    BOOST_REQUIRE(var);
    BOOST_CHECK(var->isInteger());
    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    auto ct = pb->lookupConstraint("componentToto.ct1");
    BOOST_REQUIRE(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
    BOOST_CHECK_EQUAL(ct->getUb(), 3);
    BOOST_CHECK_EQUAL(ct->getCoefficient(var), 1);
}

BOOST_AUTO_TEST_CASE(ct_with_ten_vars__pb_contains_ten_ct)
{
    // var1 <= 3
    auto var_node = variable("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodeRegistry.create<LessThanOrEqualNode>(var_node, three);

    createModel("model",
                {},
                {{"var1", ValueType::BOOL, literal(-5), literal(10), true, false}},
                {{"ct1", ct_node}});
    createComponent("model", "componentToto");
    constexpr unsigned int last_time_step = 9;
    std::vector<unsigned int> timeSteps(last_time_step + 1);
    std::ranges::generate(timeSteps, [i = 0]() mutable { return i++; });
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), 10);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 10);

    for (unsigned i = 0; i < nb_var; i++)
    {
        auto ct = pb->lookupConstraint("componentToto.ct1_" + to_string(i));
        BOOST_REQUIRE(ct);
        BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
        BOOST_CHECK_EQUAL(ct->getUb(), 3);
        auto var = pb->lookupVariable("componentToto.var1_s0_t" + to_string(i));
        BOOST_REQUIRE(var);
        BOOST_CHECK(var->isInteger());
        BOOST_CHECK_EQUAL(ct->getCoefficient(var), 1);
    }
}

BOOST_AUTO_TEST_CASE(ct_with_drop_out_of_bounds_processing_skips_out_of_bounds_timesteps)
{
    const auto runCase = [&](int leftShift,
                             int rightShift,
                             unsigned expectedCount,
                             std::array<bool, 3> expectedConstraints)
    {
        LinearProblemBuildingFixture fixture;
        auto* currentVar = fixture.variable("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);
        const auto makeNode = [&](int shift) -> Node*
        {
            if (shift == 0)
            {
                return currentVar;
            }
            return fixture.nodeRegistry.create<TimeShiftNode>(currentVar, fixture.literal(shift));
        };
        auto* ctNode = fixture.nodeRegistry.create<EqualNode>(makeNode(leftShift),
                                                              makeNode(rightShift));

        fixture.createModel(
          "model",
          {},
          {{"var1", ValueType::FLOAT, fixture.literal(-5), fixture.literal(10), true, false}},
          {{"ct1", ctNode, OutOfBoundsProcessingMode::DROP}});
        fixture.createComponent("model", "componentToto");
        FillContext ctx{0, 2, 0, 2, 0};
        fixture.buildLinearProblem(ctx);

        BOOST_REQUIRE_EQUAL(fixture.pb->constraintCount(), expectedCount);
        for (unsigned i = 0; i < expectedConstraints.size(); ++i)
        {
            BOOST_CHECK_EQUAL(fixture.pb->lookupConstraint("componentToto.ct1_" + to_string(i))
                                != nullptr,
                              expectedConstraints[i]);
        }
    };
    // var1[t+1] = var1 => 2 constraints
    runCase(1, 0, 2, {true, true, false});
    // var1[t-1] = var1 => 2 constraint
    runCase(-1, 0, 2, {false, true, true});
    // var1[t-1] = var1[t+1] => 1 constraint
    runCase(-1, 1, 1, {false, true, false});
    // var1[t+2] = var1 => 1 constraint
    runCase(2, 0, 1, {true, false, false});
    // NB : var1 = var1[t]
}

/**
 * @brief Test case for ensuring that variable bounds are correctly set using a time-series.
 *
 * This test ensures that:
 * - A variable `var1` has bounds that are set according to the given time-series values.
 * - A constraint `5 - var1 <= 3` is correctly implemented.
 * - A linear problem is built with time-series constraints.
 * - The problem contains the expected number of variables and constraints.
 * - Each constraint is correctly linked to its corresponding variable.
 *
 * @details
 * Steps performed in the test:
 * 1. Create a variable `var1` which varies in time.
 * 2. Define a constraint `5 - var1 <= 3`.
 * 3. Initialize a model with system model parameters including time-dependent bounds.
 * 4. Create a component using a time-series parameter.
 * 5. Build the linear problem using time-series data.
 * 6. Verify that the number of variables and constraints matches expectations.
 * 7. Validate each constraint's bounds, coefficient, and associated variable properties.
 * 8. Ensure that variable bounds match the time step values.
 */
BOOST_AUTO_TEST_CASE(ct_with_time_series_variable_bounds)
{
    // 5 - var1 <= 3
    auto var_node = variable("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodeRegistry.create<LessThanOrEqualNode>(
      nodeRegistry.create<SubtractionNode>(literal(5), var_node),
      three);

    createModelWithSystemModelParameter(
      "model",
      {Parameter{"bounds", TimeDependent::YES, ScenarioDependent::NO}},
      {{"var1",
        ValueType::BOOL,
        parameter("bounds", VariabilityType::VARYING_IN_TIME_ONLY),
        parameter("bounds", VariabilityType::VARYING_IN_TIME_ONLY),
        true,
        false}},
      {{"ct1", ct_node}});

    createComponent(
      "model",
      "componentToto",
      {build_context_parameter_with("bounds", "bounds", VariabilityType::VARYING_IN_TIME_ONLY)});

    const vector<unsigned int> timeSteps{0, 1};
    FillContext ctx{timeSteps.at(0), timeSteps.at(1), timeSteps.at(0), timeSteps.at(1), 0};
    auto bounds_time_series = std::make_unique<TimeSeriesSet>("bounds", 3);
    // setting 3 hours (including h 1 and 2)
    bounds_time_series->add({0., 1., 2.});
    LinearProblemData data;
    data.addDataSeries(std::move(bounds_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    buildLinearProblem(ctx, data, scenarios);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 2

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 2);

    for (const auto t: timeSteps)
    {
        auto ct = pb->lookupConstraint("componentToto.ct1_" + to_string(t));
        BOOST_REQUIRE(ct);
        BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
        BOOST_CHECK_EQUAL(ct->getUb(), -5 + 3);
        auto var = pb->lookupVariable("componentToto.var1_s0_t" + to_string(t));
        BOOST_REQUIRE(var);
        BOOST_CHECK(var->isInteger());
        BOOST_CHECK_EQUAL(ct->getCoefficient(var), -1);
        BOOST_CHECK_EQUAL(var->getLb(), t);
        BOOST_CHECK_EQUAL(var->getUb(), t);
    }
}

BOOST_AUTO_TEST_CASE(get_timeseriesNumber_for_given_year)
{
    auto var_node = variable("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodeRegistry.create<LessThanOrEqualNode>(
      nodeRegistry.create<SubtractionNode>(literal(5), var_node),
      three);

    createModelWithSystemModelParameter(
      "model",
      {Parameter{"bounds", TimeDependent::YES, ScenarioDependent::NO}},
      {{"var1",
        ValueType::BOOL,
        parameter("bounds", VariabilityType::VARYING_IN_TIME_ONLY),
        parameter("bounds", VariabilityType::VARYING_IN_TIME_ONLY),
        true,
        false}},
      {{"ct1", ct_node}});

    createComponent(
      "model",
      "componentToto",
      {build_context_parameter_with("bounds", "bounds", VariabilityType::VARYING_IN_TIME_ONLY)},
      "GROUPENAME");

    const vector<unsigned int> timeSteps{0, 1};
    FillContext ctx{timeSteps.at(0), timeSteps.at(1), timeSteps.at(0), timeSteps.at(1), 3};

    auto bounds_time_series = std::make_unique<TimeSeriesSet>("bounds", 3);
    // setting 3 hours (including h 1 and 2)
    bounds_time_series->add({1., 1., 1.});
    bounds_time_series->add({10., 10., 10.}); //<--- on voudra ça, TS=2 (commence à 1)
    bounds_time_series->add({3., 3., 3.});
    bounds_time_series->add({4., 4., 4.});

    auto scenario = std::make_unique<Scenario>("GROUPENAME");
    scenario->setTimeSerieNumber(0, 10);
    scenario->setTimeSerieNumber(1, 11);
    scenario->setTimeSerieNumber(2, 12);
    scenario->setTimeSerieNumber(3, 2); // <--- on voudra ça

    LinearProblemData data;
    data.addDataSeries(std::move(bounds_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    scenarios.emplace_back(std::move(scenario));
    buildLinearProblem(ctx, data, scenarios);

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 2);

    for (const auto t: timeSteps)
    {
        const auto* ct = pb->lookupConstraint("componentToto.ct1_" + to_string(t));
        BOOST_REQUIRE(ct);
        BOOST_CHECK_EQUAL(ct->getLb(), -pb->infinity());
        BOOST_CHECK_EQUAL(ct->getUb(), -5 + 3);
        const auto* var = pb->lookupVariable("componentToto.var1_s3_t" + to_string(t));
        BOOST_REQUIRE(var);
        BOOST_CHECK(var->isInteger());
        BOOST_CHECK_EQUAL(ct->getCoefficient(var), -1);
        BOOST_CHECK_EQUAL(var->getLb(), 10);
        BOOST_CHECK_EQUAL(var->getUb(), 10);
    }
}

BOOST_AUTO_TEST_CASE(ct_one_var_with_coef_pb_contains_the_ct)
{
    // 3 * var1 >= 5 * var1 + 5
    // simplified to : -2 * var1 >= 5
    auto var_node = variable("var__1", 0);
    auto five = literal(5);
    auto three = literal(3);
    auto coef_node_left = multiply(three, var_node);
    auto coef_node_right = multiply(var_node, five);
    auto sum_node_right = nodeRegistry.create<SumNode>(coef_node_right, five);
    auto ct_node = nodeRegistry.create<GreaterThanOrEqualNode>(coef_node_left, sum_node_right);

    createModelWithOneFloatVar("model",
                               {},
                               "var__1",
                               literal(-5),
                               literal(10),
                               {{"ct_1", ct_node}});
    createComponent("model", "componentTata");
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentTata.var__1"));
    auto var = pb->lookupVariable("componentTata.var__1");
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupConstraint("componentTata.ct_1"));
    auto ct = pb->lookupConstraint("componentTata.ct_1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), 5);
    BOOST_CHECK_EQUAL(ct->getUb(), pb->infinity());
    BOOST_CHECK_EQUAL(ct->getCoefficient(var), -2);
}

BOOST_AUTO_TEST_CASE(ct_with_two_vars)
{
    // param1(-16) * v1 + 8 * v2 + 5 - param2(8) = 7 * v1 +  param3(5) * v2 + 89 + 5 * param4(-3)
    // simplifies to: -23 * v1 + 3 * v2 = 77
    vector<string> params = {"param1", "param2", "param3", "param4"};
    auto threeHundreds = literal(300.);
    auto minus50 = literal(-50.);
    VariableData var1Data = {"v1", ValueType::FLOAT, minus50, threeHundreds, false, false};
    auto sixty = literal(60.);
    auto seventy5 = literal(75.);
    VariableData var2Data = {"v2", ValueType::FLOAT, sixty, seventy5, false, false};

    auto v1 = variable("v1", 0);
    auto param1 = parameter("param1");
    auto v2 = variable("v2", 1);
    auto param2 = parameter("param2");
    auto sum_node_left = nodeRegistry.create<SumNode>(multiply(v1, param1),
                                                      multiply(literal(8), v2),
                                                      literal(5),
                                                      negate(param2));
    auto param3 = parameter("param3");
    auto param4 = parameter("param4");
    auto sum_node_right = nodeRegistry.create<SumNode>(multiply(v1, literal(7)),
                                                       multiply(param3, v2),
                                                       literal(89),
                                                       multiply(literal(5), param4));
    auto ct_node = nodeRegistry.create<EqualNode>(sum_node_left, sum_node_right);

    createModel("my_new_model", params, {var1Data, var2Data}, {{"constraint1", ct_node}});
    createComponent("my_new_model",
                    "my_component",
                    {build_context_parameter_with("param1", "-16"),
                     build_context_parameter_with("param2", "8"),
                     build_context_parameter_with("param3", "5"),
                     build_context_parameter_with("param4", "-3")});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupConstraint("my_component.constraint1"));
    auto ct = pb->lookupConstraint("my_component.constraint1");
    BOOST_CHECK(ct);
    BOOST_CHECK_EQUAL(ct->getLb(), 77);
    BOOST_CHECK_EQUAL(ct->getUb(), 77);
    auto* cv1 = pb->lookupVariable("my_component.v1");
    BOOST_REQUIRE(cv1);
    BOOST_CHECK_EQUAL(ct->getCoefficient(cv1), -23);
    auto* cv2 = pb->lookupVariable("my_component.v2");
    BOOST_REQUIRE(cv2);
    BOOST_CHECK_EQUAL(ct->getCoefficient(cv2), 3);
}

BOOST_AUTO_TEST_CASE(two_constraints__they_are_created)
{
    auto minus50 = literal(-50.);
    auto threeHundred = literal(300.);
    // 3 * v1 -2 <= v2 (simplifies to : 3 * v1 - 2 * v2 <= 2)
    // v2 <= v1 / 2 (simplifies to : -0.5 * v1 + v2 <= 0)
    VariableData var1Data = {"v1", ValueType::FLOAT, minus50, threeHundred, false, false};
    auto sixty = literal(60.);
    auto seventy5 = literal(75.);
    VariableData var2Data = {"v2", ValueType::FLOAT, sixty, seventy5, false, false};

    auto v1 = variable("v1", 0);
    auto three = literal(3);
    auto two_1 = literal(2);
    auto v2 = variable("v2", 1);
    auto ct1_node = nodeRegistry.create<LessThanOrEqualNode>(
      nodeRegistry.create<SubtractionNode>(multiply(three, v1), two_1),
      v2);
    auto two_2 = literal(2);
    auto ct2_node = nodeRegistry.create<LessThanOrEqualNode>(
      v2,
      nodeRegistry.create<DivisionNode>(v1, two_2));

    createModel("my_new_model", {}, {var1Data, var2Data}, {{"ct1", ct1_node}, {"ct2", ct2_node}});
    createComponent("my_new_model", "my_component", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 2);

    BOOST_CHECK_NO_THROW((void)pb->lookupConstraint("my_component.ct1"));
    auto ct1 = pb->lookupConstraint("my_component.ct1");
    BOOST_CHECK(ct1);
    BOOST_CHECK_EQUAL(ct1->getLb(), -numeric_limits<float>::infinity());
    BOOST_CHECK_EQUAL(ct1->getUb(), 2);
    {
        auto* cv1 = pb->lookupVariable("my_component.v1");
        BOOST_REQUIRE(cv1);
        BOOST_CHECK_EQUAL(ct1->getCoefficient(cv1), 3);
        auto* cv2 = pb->lookupVariable("my_component.v2");
        BOOST_REQUIRE(cv2);
        BOOST_CHECK_EQUAL(ct1->getCoefficient(cv2), -1);
    }

    BOOST_CHECK_NO_THROW((void)pb->lookupConstraint("my_component.ct2"));
    auto ct2 = pb->lookupConstraint("my_component.ct2");
    BOOST_REQUIRE(ct2);
    BOOST_CHECK_EQUAL(ct2->getLb(), -numeric_limits<float>::infinity());
    BOOST_CHECK_EQUAL(ct2->getUb(), 0);
    {
        auto* cv1 = pb->lookupVariable("my_component.v1");
        BOOST_REQUIRE(cv1);
        BOOST_CHECK_EQUAL(ct2->getCoefficient(cv1), -0.5);

        auto* cv2 = pb->lookupVariable("my_component.v2");
        BOOST_REQUIRE(cv2);
        BOOST_CHECK_EQUAL(ct2->getCoefficient(cv2), 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
