// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/optimisation/linear-problem-data-impl/timeSeriesSet.h>
#include <antares/study/system-model/variabilityType.h>
#include "antares/exception/RuntimeError.hpp"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/study/system-model/component.h"

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

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_getObjectiveCoefficient_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(one_const_var_with_objective)
{
    auto objective = variable("x", 0);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.x"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.x")), 1);
}

BOOST_AUTO_TEST_CASE(one_time_dependent_var_with_objective)
{
    auto objective = variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    BOOST_CHECK_THROW(buildLinearProblem(ctx), Antares::Error::RuntimeError);
}

BOOST_AUTO_TEST_CASE(two_const_vars_but_only_one_in_objective)
{
    VariableData var1Data = {"v1", ValueType::FLOAT, literal(-50.), literal(300.), false, false};
    VariableData var2Data = {"v2", ValueType::FLOAT, literal(60.), literal(75.), false, false};
    auto objective = multiply(variable("v2", 1), literal(37));

    createModel("model", {}, {var1Data, var2Data}, {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.v1"));
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.v2"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.v1")), 0);
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.v2")), 37);
}

BOOST_AUTO_TEST_CASE(time_sum_var_with_objective)
{
    // Objective: sum(2*x + 3) over time steps 0 to 2
    // This should give coefficient 2 for each variable x_t0, x_t1, x_t2
    auto expression = add(multiply(literal(2),
                                   variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY)),
                          literal(3));
    auto timeSum = nodeRegistry.create<AllTimeSumNode>(expression);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, timeSum, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 2;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 3

    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    for (unsigned i = 0; i < nb_var; i++)
    {
        const auto var_name = "componentA.x_s0_t" + to_string(i);
        BOOST_CHECK_NO_THROW((void)pb->lookupVariable(var_name));
        BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable(var_name)), 2);
        // Offset 9
    }
}

BOOST_AUTO_TEST_CASE(objective_varying_in_time_not_supported_error)
{
    // Objective: 2*x + 3
    auto expression = add(multiply(literal(2),
                                   variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY)),
                          literal(3));

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, expression, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 2;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    BOOST_CHECK_THROW(buildLinearProblem(ctx), Antares::Error::RuntimeError);
}

BOOST_AUTO_TEST_CASE(one_var_with_param_objective)
{
    // -param(5)*param(5) * x
    auto objective = multiply(negate(multiply(parameter("param"), parameter("param"))),
                              variable("x", 0));
    createModelWithOneFloatVar("model", {"param"}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {build_context_parameter_with("param", "5")});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.x"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.x")), -25);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_getObjectiveOffset_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(one_var_no_offset_expect_objective_offset_zero)
{
    // exp: ax + b, a = 1, b = 0
    auto objective = variable("x", 0);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 0);
}

BOOST_AUTO_TEST_CASE(one_var_with_param_no_offset_expect_objective_offset_zero)
{
    // exp: param * x, param = 5, no offset
    auto objective = multiply(parameter("param"), variable("x", 0));
    createModelWithOneFloatVar("model", {"param"}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {build_context_parameter_with("param", "5")});
    buildLinearProblem();
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 0);
}

BOOST_AUTO_TEST_CASE(one_var_with_constant_offset_offset_expect_objective_offset_ten)
{
    // exp: ax + b, a = 1, b = 10
    const auto objective = add(variable("x", 0), literal(10));
    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 10);
}

BOOST_AUTO_TEST_CASE(one_param_offset_expect_objective_offset_five)
{
    // exp: x + param, param = 5
    auto objective = parameter("param");
    createModelWithOneFloatVar("model", {"param"}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {build_context_parameter_with("param", "5")});
    FillContext ctx{0, 10, 0, 10, 0};
    buildLinearProblem(ctx);
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 5);
}

BOOST_AUTO_TEST_CASE(
  one_time_dependent_var_ten_timesteps_with_constant_offset_ten_expected_objective_offset_ten_times_nb_timesteps)
{
    // exp: sum(ax + b), a: [t0,...tn], b = 10
    auto objective = nodeRegistry.create<AllTimeSumNode>(
      add(variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY), literal(10)));

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), nb_var * 10); // 10 timesteps each with offset 10
}

BOOST_AUTO_TEST_CASE(
  one_var_with_time_dependent_offset_3_timesteps_expected_objective_offset_sum_of_param_values_at_time_steps)
{
    // exp: sum(x + param(t)), param = [10,11,12]
    auto objective = nodeRegistry.create<AllTimeSumNode>(
      add(variable("x", 0), parameter("param", VariabilityType::VARYING_IN_TIME_ONLY)));
    createModelWithSystemModelParameter(
      "model",
      {Parameter{"param", TimeDependent::YES, ScenarioDependent::NO}},
      {{"x", ValueType::FLOAT, literal(-5), literal(10), true, false}},
      {},
      objective);
    createComponent(
      "model",
      "componentA",
      {build_context_parameter_with("param", "bounds", VariabilityType::VARYING_IN_TIME_ONLY)});

    FillContext ctx{0, 2, 0, 2, 0};
    auto bounds_time_series = std::make_unique<TimeSeriesSet>("bounds", 3);
    // setting 3 hours (including h 1 and 2)
    bounds_time_series->add({10., 11., 12.});
    LinearProblemData data;
    data.addDataSeries(std::move(bounds_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    buildLinearProblem(ctx, data, scenarios);
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 33); // 10 + 11 + 12
}

BOOST_AUTO_TEST_CASE(
  one_var_with_scenario_dependent_offset_expected_objective_offset_value_for_scenario)
{
    // exp: x + param(s), param varies by scenario
    auto objective = Sum(
      add(variable("x", 0), parameter("param", VariabilityType::VARYING_IN_SCENARIO_ONLY)));
    createModelWithSystemModelParameter(
      "model",
      {Parameter{"param", TimeDependent::NO, ScenarioDependent::YES}},
      {{"x", ValueType::FLOAT, literal(-5), literal(10), false, false}},
      {},
      objective);
    createComponent(
      "model",
      "componentA",
      {build_context_parameter_with("param", "bounds", VariabilityType::VARYING_IN_SCENARIO_ONLY)},
      "scenario_group");

    FillContext ctx{0, 0, 0, 0, 0};
    auto bounds_time_series = std::make_unique<TimeSeriesSet>("bounds", 1);
    // Add 3 different scenarios with different offset values
    bounds_time_series->add({15.}); // scenario 0
    bounds_time_series->add({25.}); // scenario 1
    bounds_time_series->add({35.}); // scenario 2
    LinearProblemData data;
    data.addDataSeries(std::move(bounds_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    auto scenario0 = std::make_unique<Scenario>("scenario_group");
    scenario0->setTimeSerieNumber(0, 3);
    scenarios.push_back(std::move(scenario0));

    buildLinearProblem(ctx, data, scenarios);
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 35); // value for scenario_group ts number 3
}

BOOST_AUTO_TEST_CASE(
  one_var_with_time_and_scenario_dependent_offset_expected_objective_offset_sum_for_all_time_and_scenarios)
{
    // exp: sum(x + param(t,s)), param varies by both time and scenario
    auto objective = Sum(
      add(variable("x", 0), parameter("param", VariabilityType::VARYING_IN_TIME_AND_SCENARIO)));
    createModelWithSystemModelParameter(
      "model",
      {Parameter{"param", TimeDependent::YES, ScenarioDependent::YES}},
      {{"x", ValueType::FLOAT, literal(-5), literal(10), true, false}},
      {},
      objective);
    createComponent(
      "model",
      "componentA",
      {build_context_parameter_with("param", "bounds", VariabilityType::VARYING_IN_TIME_ONLY)},
      "scenario_group");

    FillContext ctx{0, 2, 0, 2, 0}; // 3 time steps
    auto bounds_time_series = std::make_unique<TimeSeriesSet>("bounds", 3);
    // Add 2 different scenarios with different offset values across time
    // Scenario 0: time series [10, 20, 30]
    bounds_time_series->add({10., 20., 30.});
    // Scenario 1: time series [15, 25, 35]
    bounds_time_series->add({15., 25., 35.});
    LinearProblemData data;
    data.addDataSeries(std::move(bounds_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    auto scenario0 = std::make_unique<Scenario>("scenario_group");
    scenario0->setTimeSerieNumber(0, 2);
    scenarios.push_back(std::move(scenario0));

    buildLinearProblem(ctx, data, scenarios);
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 75);
}

BOOST_AUTO_TEST_CASE(var_and_param_both_varying_in_time_and_scenario_with_different_scenario_groups)
{
    // exp: sum(x(t,s_x) + param(t,s_y))
    // Variable x varies in time and scenario (uses scenarioX)
    // Param varies in time and scenario (uses scenarioY)
    auto objective = Sum(add(variable("x", 0, VariabilityType::VARYING_IN_TIME_AND_SCENARIO),
                             parameter("param", VariabilityType::VARYING_IN_TIME_AND_SCENARIO)));
    createModelWithSystemModelParameter(
      "model",
      {Parameter{"param", TimeDependent::YES, ScenarioDependent::YES}},
      {{"x", ValueType::FLOAT, literal(-5), literal(100), true, true}},
      {},
      objective);
    createComponent(
      "model",
      "componentA",
      {build_context_parameter_with("param", "param_ts", VariabilityType::VARYING_IN_TIME_ONLY)},
      "scenarioX");

    FillContext ctx{0, 2, 0, 2, 0}; // 3 time steps

    // Time series for variable x (scenarioX group)
    auto x_time_series = std::make_unique<TimeSeriesSet>("x", 3);
    x_time_series->add({1., 2., 3.});    // time series 0
    x_time_series->add({11., 22., 33.}); // time series 1

    // Time series for parameter (scenarioY group)
    auto param_time_series = std::make_unique<TimeSeriesSet>("param_ts", 3);
    param_time_series->add({3., 6., 9.}); // time series 0

    LinearProblemData data;
    data.addDataSeries(std::move(x_time_series));
    data.addDataSeries(std::move(param_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    // Scenario for variable x: select second time series [11, 22, 33]
    auto scenarioX = std::make_unique<Scenario>("scenarioX");
    scenarioX->setTimeSerieNumber(0, 1); // Select time series 1
    scenarios.push_back(std::move(scenarioX));

    // Scenario for parameter: select first time series [3, 6, 9]
    auto scenarioY = std::make_unique<Scenario>("scenarioY");
    scenarioY->setTimeSerieNumber(0, 0); // Select time series 0
    scenarios.push_back(std::move(scenarioY));

    buildLinearProblem(ctx, data, scenarios);

    // Variable x contributes: 11 + 22 + 33 = 66
    // Parameter contributes: 3 + 6 + 9 = 18
    // But only the parameter is in the objective offset (variable coefficients don't contribute to
    // offset) Expected: 18 (only from parameter offset)
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 18);
}

BOOST_AUTO_TEST_CASE(two_expressions_one_with_time_varying_param_one_with_constant_offset)
{
    // Expression 1: sum(x + param(t), param = [1, 2, 3])
    // Expression 2: sum(x + 20)
    auto expr1 = Sum(add(variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY),
                         parameter("param", VariabilityType::VARYING_IN_TIME_ONLY)));
    auto expr2 = Sum(add(variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY), literal(20)));
    auto objective = add(expr1, expr2);

    createModelWithSystemModelParameter(
      "model",
      {Parameter{"param", TimeDependent::YES, ScenarioDependent::NO}},
      {{"x", ValueType::FLOAT, literal(-5), literal(100), true, false}},
      {},
      objective);
    createComponent(
      "model",
      "componentA",
      {build_context_parameter_with("param", "param_ts", VariabilityType::VARYING_IN_TIME_ONLY)});

    FillContext ctx{0, 2, 0, 2, 0}; // 3 time steps

    // Time series for parameter: [1, 2, 3]
    auto param_time_series = std::make_unique<TimeSeriesSet>("param_ts", 3);
    param_time_series->add({1., 2., 3.});

    LinearProblemData data;
    data.addDataSeries(std::move(param_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    buildLinearProblem(ctx, data, scenarios);

    // Expression 1 offset: 1 + 2 + 3 = 6
    // Expression 2 offset: 20 + 20 + 20 = 60
    // Total offset: 6 + 60 = 66
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 66);
}

BOOST_AUTO_TEST_CASE(multiple_objectives_in_model)
{
    // Model with two objectives:
    // Objective 1: x + 10
    // Objective 2: x + param(t), param = [5, 10, 15]
    // Expected offset over 3 time steps: (10+10+10) + (5+10+15) = 30 + 30 = 60

    auto objective1 = Sum(
      add(variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY), literal(10)));
    auto objective2 = Sum(add(variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY),
                              parameter("param", VariabilityType::VARYING_IN_TIME_ONLY)));

    std::vector objectives = {objective1, objective2};

    createModelWithMultipleObjectives(
      "model",
      {Parameter{"param", TimeDependent::YES, ScenarioDependent::NO}},
      {{"x", ValueType::FLOAT, literal(-5), literal(100), true, false}},
      {},
      objectives);
    createComponent(
      "model",
      "componentA",
      {build_context_parameter_with("param", "param_ts", VariabilityType::VARYING_IN_TIME_ONLY)});

    FillContext ctx{0, 2, 0, 2, 0}; // 3 time steps

    // Time series for parameter: [5, 10, 15]
    auto param_time_series = std::make_unique<TimeSeriesSet>("param_ts", 3);
    param_time_series->add({5., 10., 15.});

    LinearProblemData data;
    data.addDataSeries(std::move(param_time_series));

    std::vector<std::unique_ptr<IScenario>> scenarios;
    buildLinearProblem(ctx, data, scenarios);

    // Objective 1 offset: 10 + 10 + 10 = 30
    // Objective 2 offset: 5 + 10 + 15 = 30
    // Total offset: 30 + 30 = 60
    BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 60);
}

BOOST_AUTO_TEST_CASE(time_sum_var_with_objective)
{
    // Objective: sum(2*x + 3) over time steps 0 to 2
    // This should give coefficient 2 for each variable x_t0, x_t1, x_t2
    auto from = literal(0);
    auto to = literal(2);
    auto expression = add(multiply(literal(2),
                                   variable("x", 0, VariabilityType::VARYING_IN_TIME_ONLY)),
                          literal(3));
    auto timeSum = nodeRegistry.create<Nodes::AllTimeSumNode>(expression);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, timeSum, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 2;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 3

    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    for (unsigned i = 0; i < nb_var; i++)
    {
        {
            BOOST_CHECK_EQUAL(pb->getObjectiveOffset(), 9);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
