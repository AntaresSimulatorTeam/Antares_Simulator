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

#include <boost/test/unit_test.hpp>

#include "antares/exception/RuntimeError.hpp"
#include "antares/expressions/nodes/ExpressionsNodes.h"
#include "antares/expressions/visitors/TimeIndex.h"
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
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimization;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Nodes;
using namespace Test::Modeler;
using namespace std;

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addVariables_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(var_with_literal_bounds_to_filler__problem_contains_one_var)
{
    createModelWithOneFloatVar("some_model", {}, "var1", literal(-5), literal(10), {});
    createComponent("some_model", "some_component");
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->lookupVariable("some_component.var1");
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), -5);
    BOOST_CHECK_EQUAL(var->getUb(), 10);
    BOOST_CHECK(!var->isInteger());
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 0);
}

BOOST_AUTO_TEST_CASE(ten_timesteps_var_with_literal_bounds_to_filler__problem_contains_ten_vars)
{
    createModelWithOneFloatVar("some_model",
                               {},
                               "var1",
                               literal(-5),
                               literal(10),
                               {},
                               nullptr,
                               true);
    createComponent("some_model", "some_component");
    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step, 0, 0, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 10
    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    for (unsigned int i = 0; i < nb_var; i++)
    {
        auto* var = pb->lookupVariable("some_component.var1_s0_t" + to_string(i));
        BOOST_REQUIRE(var);
        BOOST_CHECK_EQUAL(var->getLb(), -5);
        BOOST_CHECK_EQUAL(var->getUb(), 10);
        BOOST_CHECK(!var->isInteger());
        BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 0);
    }
}

BOOST_AUTO_TEST_CASE(var_with_wrong_parameter_lb__exception_is_raised)
{
    createModel("my-model",
                {},
                {{"variable", ValueType::FLOAT, parameter("parameter-not-in-model"), literal(10)}},
                {});
    createComponent("my-model", "my-component");
    // TODO : improve exception message in eval visitor
    BOOST_CHECK_THROW(buildLinearProblem(), out_of_range);
}

BOOST_AUTO_TEST_CASE(var_with_empty_lower_bound_default_to_minus_infinity)
{
    createModel("my-model",
                {},
                {{"variableF", ValueType::FLOAT, nullptr, literal(10)},
                 {"variableI", ValueType::INTEGER, nullptr, literal(10)}},
                {});
    createComponent("my-model", "component");
    buildLinearProblem();
    auto* var = pb->lookupVariable("component.variableF_s0_t" + to_string(0));
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), -std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(var->getUb(), 10);

    var = pb->lookupVariable("component.variableI_s0_t" + to_string(0));
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), -std::numeric_limits<double>::infinity());
    BOOST_CHECK_EQUAL(var->getUb(), 10);
}

BOOST_AUTO_TEST_CASE(var_with_empty_upper_bound_default_to_infinity)
{
    createModel("my-model",
                {},
                {{"variableF", ValueType::FLOAT, literal(10), nullptr},
                 {"variableI", ValueType::INTEGER, literal(10), nullptr}},
                {});
    createComponent("my-model", "component");
    buildLinearProblem();
    auto* var = pb->lookupVariable("component.variableF_s0_t" + to_string(0));
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), 10);
    BOOST_CHECK_EQUAL(var->getUb(), std::numeric_limits<double>::infinity());

    var = pb->lookupVariable("component.variableI_s0_t" + to_string(0));
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), 10);
    BOOST_CHECK_EQUAL(var->getUb(), std::numeric_limits<double>::infinity());
}

BOOST_AUTO_TEST_CASE(var_BOOLEAN_with_empty_lower_bound_default_to_0)
{
    createModel("my-model", {}, {{"variableB", ValueType::BOOL, nullptr, literal(1)}}, {});
    createComponent("my-model", "component");
    buildLinearProblem();
    auto* var = pb->lookupVariable("component.variableB_s0_t" + to_string(0));
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), 0);
    BOOST_CHECK_EQUAL(var->getUb(), 1);
}

BOOST_AUTO_TEST_CASE(var_BOOLEAN_with_empty_upper_bound_default_to_1)
{
    createModel("my-model",
                {},
                {{
                  "variableB",
                  ValueType::BOOL,
                  literal(0),
                  nullptr,
                }},
                {});
    createComponent("my-model", "component");
    buildLinearProblem();
    auto* var = pb->lookupVariable("component.variableB_s0_t" + to_string(0));
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->getLb(), 0);
    BOOST_CHECK_EQUAL(var->getUb(), 1);
}

BOOST_AUTO_TEST_CASE(two_variables_given_to_different_fillers__LP_contains_the_two_variables)
{
    createModelWithOneFloatVar("m1", {}, "var1", literal(-1), literal(6), {});
    createModelWithOneFloatVar("m2", {}, "var2", literal(-3), literal(2), {});
    createComponent("m1", "component_1");
    createComponent("m2", "component_2");
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);

    auto* var1 = pb->lookupVariable("component_1.var1");
    BOOST_REQUIRE(var1);
    BOOST_CHECK(!var1->isInteger());
    BOOST_CHECK_EQUAL(var1->getLb(), -1.);
    BOOST_CHECK_EQUAL(var1->getUb(), 6.);

    auto* var2 = pb->lookupVariable("component_2.var2");
    BOOST_REQUIRE(var2);
    BOOST_CHECK(!var2->isInteger());
    BOOST_CHECK_EQUAL(var2->getLb(), -3.);
    BOOST_CHECK_EQUAL(var2->getUb(), 2.);
}

BOOST_AUTO_TEST_CASE(
  two_times_10_variables_given_to_different_fillers__LP_contains_the_two_variables)
{
    createModelWithOneFloatVar("m1", {}, "var1", literal(-1), literal(6), {}, nullptr, true);
    createModelWithOneFloatVar("m2", {}, "var2", literal(-3), literal(2), {}, nullptr, true);
    createComponent("m1", "component_1");
    createComponent("m2", "component_2");
    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), 2 * 10);
    for (unsigned i = 0; i < nb_var; i++)
    {
        auto* var1 = pb->lookupVariable("component_1.var1_s0_t" + to_string(i));
        BOOST_REQUIRE(var1);
        BOOST_CHECK(!var1->isInteger());
        BOOST_CHECK_EQUAL(var1->getLb(), -1.);
        BOOST_CHECK_EQUAL(var1->getUb(), 6.);

        auto* var2 = pb->lookupVariable("component_2.var2_s0_t" + to_string(i));
        BOOST_REQUIRE(var2);
        BOOST_CHECK(!var2->isInteger());
        BOOST_CHECK_EQUAL(var2->getLb(), -3.);
        BOOST_CHECK_EQUAL(var2->getUb(), 2.);
    }
}

BOOST_AUTO_TEST_CASE(var_whose_bounds_are_parameters_given_to_component__problem_contains_this_var)
{
    createModel("model",
                {"pmin", "pmax"},
                {{"var1", ValueType::INTEGER, parameter("pmin"), parameter("pmax"), false, false}},
                {});
    createComponent("model",
                    "componentToto",
                    {build_context_parameter_with("pmin", "-3."),
                     build_context_parameter_with("pmax", "4.")});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->lookupVariable("componentToto.var1");
    BOOST_REQUIRE(var);
    BOOST_CHECK(var->isInteger());
    BOOST_CHECK_EQUAL(var->getLb(), -3.);
    BOOST_CHECK_EQUAL(var->getUb(), 4.);
}

BOOST_AUTO_TEST_CASE(three_different_vars__exist)
{
    VariableData var1 = {"is_cluster_on", ValueType::BOOL, literal(0), literal(1), false, false};
    VariableData var2 = {"n_started_units",
                         ValueType::INTEGER,
                         literal(0),
                         parameter("nUnits"),
                         false,
                         false};
    VariableData var3 = {"p_per_unit",
                         ValueType::FLOAT,
                         parameter("pmin"),
                         parameter("pmax"),
                         false,
                         false};
    createModel("thermalClusterModel", {"pmin", "pmax", "nUnits"}, {var1, var2, var3}, {});
    createComponent("thermalClusterModel",
                    "thermalCluster1",
                    {build_context_parameter_with("pmin", "100.248"),
                     build_context_parameter_with("pmax", "950.6784"),
                     build_context_parameter_with("nUnits", "17.")});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 3);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* is_cluster_on = pb->lookupVariable("thermalCluster1.is_cluster_on");
    BOOST_REQUIRE(is_cluster_on);
    BOOST_CHECK(is_cluster_on->isInteger());
    BOOST_CHECK_EQUAL(is_cluster_on->getLb(), 0);
    BOOST_CHECK_EQUAL(is_cluster_on->getUb(), 1);
    auto* n_started_units = pb->lookupVariable("thermalCluster1.n_started_units");
    BOOST_REQUIRE(n_started_units);
    BOOST_CHECK(n_started_units->isInteger());
    BOOST_CHECK_EQUAL(n_started_units->getLb(), 0);
    BOOST_CHECK_EQUAL(n_started_units->getUb(), 17);
    auto* p_per_unit = pb->lookupVariable("thermalCluster1.p_per_unit");
    BOOST_REQUIRE(p_per_unit);
    BOOST_CHECK(!p_per_unit->isInteger());
    BOOST_CHECK_EQUAL(p_per_unit->getLb(), 100.248);
    BOOST_CHECK_EQUAL(p_per_unit->getUb(), 950.6784);
}

BOOST_AUTO_TEST_CASE(one_model_two_components__dont_clash)
{
    createModelWithOneFloatVar("m1", {"ub"}, "var1", literal(-100), parameter("ub"), {});
    createComponent("m1", "component_1", {build_context_parameter_with("ub", "15")});
    createComponent("m1", "component_2", {build_context_parameter_with("ub", "48")});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* c1_var1 = pb->lookupVariable("component_1.var1");
    BOOST_REQUIRE(c1_var1);
    BOOST_CHECK(!c1_var1->isInteger());
    BOOST_CHECK_EQUAL(c1_var1->getLb(), -100);
    BOOST_CHECK_EQUAL(c1_var1->getUb(), 15);
    auto* c2_var1 = pb->lookupVariable("component_2.var1");
    BOOST_REQUIRE(c2_var1);
    BOOST_CHECK(!c2_var1->isInteger());
    BOOST_CHECK_EQUAL(c2_var1->getLb(), -100);
    BOOST_CHECK_EQUAL(c2_var1->getUb(), 48);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addConstraints_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(ct_one_var__pb_contains_the_ct)
{
    // var1 <= 3
    auto var_node = variable("var1");
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(var_node, three);

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
    auto var_node = variable("var1",
                             Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(var_node, three);

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
    auto var_node = variable("var1",
                             Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(nodes.create<SubtractionNode>(literal(5),
                                                                                   var_node),
                                                     three);

    createModelWithSystemModelParameter(
      "model",
      {Parameter{"bounds", TimeDependent::YES, ScenarioDependent::NO}},
      {{"var1",
        ValueType::BOOL,
        parameter("bounds", Visitors::TimeIndex::VARYING_IN_TIME_ONLY),
        parameter("bounds", Visitors::TimeIndex::VARYING_IN_TIME_ONLY),
        true,
        false}},
      {{"ct1", ct_node}});

    createComponent(
      "model",
      "componentToto",
      {build_context_parameter_with("bounds", "bounds", Visitors::ParameterType::TIMESERIE)});

    const vector<unsigned int> timeSteps{1, 2};
    FillContext ctx{timeSteps.at(0), timeSteps.at(1), timeSteps.at(0), timeSteps.at(1), 0};

    auto bounds_time_series = std::make_unique<TimeSeriesSet>("bounds", 3);
    // setting 3 hours (including h 1 and 2)
    bounds_time_series->add({1. * timeSteps.at(0), 1. * timeSteps.at(0), 1. * timeSteps.at(1)});
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
    auto var_node = variable("var1",
                             Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);
    auto three = literal(3);
    auto ct_node = nodes.create<LessThanOrEqualNode>(nodes.create<SubtractionNode>(literal(5),
                                                                                   var_node),
                                                     three);

    createModelWithSystemModelParameter(
      "model",
      {Parameter{"bounds", TimeDependent::YES, ScenarioDependent::NO}},
      {{"var1",
        ValueType::BOOL,
        parameter("bounds", Visitors::TimeIndex::VARYING_IN_TIME_ONLY),
        parameter("bounds", Visitors::TimeIndex::VARYING_IN_TIME_ONLY),
        true,
        false}},
      {{"ct1", ct_node}});

    createComponent(
      "model",
      "componentToto",
      {build_context_parameter_with("bounds", "bounds", Visitors::ParameterType::TIMESERIE)},
      "GROUPENAME");

    const vector<unsigned int> timeSteps{1, 2};
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
    auto var_node = variable("var__1");
    auto five = literal(5);
    auto three = literal(3);
    auto coef_node_left = multiply(three, var_node);
    auto coef_node_right = multiply(var_node, five);
    auto sum_node_right = nodes.create<SumNode>(coef_node_right, five);
    auto ct_node = nodes.create<GreaterThanOrEqualNode>(coef_node_left, sum_node_right);

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

    auto v1 = variable("v1");
    auto param1 = parameter("param1");
    auto v2 = variable("v2");
    auto param2 = parameter("param2");
    auto sum_node_left = nodes.create<SumNode>(multiply(v1, param1),
                                               multiply(literal(8), v2),
                                               literal(5),
                                               negate(param2));
    auto param3 = parameter("param3");
    auto param4 = parameter("param4");
    auto sum_node_right = nodes.create<SumNode>(multiply(v1, literal(7)),
                                                multiply(param3, v2),
                                                literal(89),
                                                multiply(literal(5), param4));
    auto ct_node = nodes.create<EqualNode>(sum_node_left, sum_node_right);

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

    auto v1 = variable("v1");
    auto three = literal(3);
    auto two_1 = literal(2);
    auto v2 = variable("v2");
    auto ct1_node = nodes.create<LessThanOrEqualNode>(
      nodes.create<SubtractionNode>(multiply(three, v1), two_1),
      v2);
    auto two_2 = literal(2);
    auto ct2_node = nodes.create<LessThanOrEqualNode>(v2, nodes.create<DivisionNode>(v1, two_2));

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

BOOST_FIXTURE_TEST_SUITE(_ComponentFiller_addObjective_, LinearProblemBuildingFixture)

BOOST_AUTO_TEST_CASE(one_var_with_objective)
{
    auto objective = variable("x");

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.x"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.x")), 1);
}

BOOST_AUTO_TEST_CASE(one_time_dependent_var_with_objective)
{
    auto objective = variable("x", Antares::Expressions::Visitors::TimeIndex::VARYING_IN_TIME_ONLY);

    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective, true);
    createComponent("model", "componentA", {});

    constexpr unsigned int last_time_step = 9;
    FillContext ctx{0, last_time_step, 0, last_time_step, 0};
    buildLinearProblem(ctx);
    const auto nb_var = ctx.getLocalNumberOfTimeSteps(); // = 10

    BOOST_CHECK_EQUAL(pb->variableCount(), nb_var);
    for (unsigned i = 0; i < nb_var; i++)
    {
        const auto var_name = "componentA.x_s0_t" + to_string(i);
        BOOST_CHECK_NO_THROW((void)pb->lookupVariable(var_name));
        BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable(var_name)), 1);
    }
}

BOOST_AUTO_TEST_CASE(two_vars_but_only_one_in_objective)
{
    VariableData var1Data = {"v1", ValueType::FLOAT, literal(-50.), literal(300.), false, false};
    VariableData var2Data = {"v2", ValueType::FLOAT, literal(60.), literal(75.), false, false};
    auto objective = multiply(variable("v2"), literal(37));

    createModel("model", {}, {var1Data, var2Data}, {}, objective);
    createComponent("model", "componentA", {});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 2);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.v1"));
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.v2"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.v1")), 0);
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.v2")), 37);
}

BOOST_AUTO_TEST_CASE(one_var_with_param_objective)
{
    // -param(5)*param(5) * x
    auto objective = multiply(negate(multiply(parameter("param"), parameter("param"))),
                              variable("x"));
    createModelWithOneFloatVar("model", {"param"}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {build_context_parameter_with("param", "5")});
    buildLinearProblem();

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_NO_THROW((void)pb->lookupVariable("componentA.x"));
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(pb->lookupVariable("componentA.x")), -25);
}

BOOST_AUTO_TEST_CASE(offset_in_objective__throws_exception)
{
    auto objective = literal(6);
    createModelWithOneFloatVar("model", {}, "x", literal(-50), literal(-40), {}, objective);
    createComponent("model", "componentA", {});
    BOOST_CHECK_EXCEPTION(buildLinearProblem(),
                          invalid_argument,
                          checkMessage("Antares does not support objective offsets (found in model "
                                       "'model' of component 'componentA')."));
}

// Mock classes
class MockMipVariable: public IMipVariable
{
public:
    MockMipVariable(double lb, double ub, bool integer, const std::string& name):
        lb_(lb),
        ub_(ub),
        integer_(integer),
        name_(name)
    {
    }

    bool isInteger() const override
    {
        return integer_;
    }

    void setLb(double lb) override
    {
        lb_ = lb;
    }

    void setUb(double ub) override
    {
        ub_ = ub;
    }

    void setBounds(double lb, double ub) override
    {
        lb_ = lb;
        ub_ = ub;
    }

    double getLb() const override
    {
        return lb_;
    }

    double getUb() const override
    {
        return ub_;
    }

    const std::string& getName() const override
    {
        return name_;
    }

    MipBasisStatus getMipBasisStatus() const override
    {
        return MipBasisStatus::FREE;
    }

    double solutionValue() const override
    {
        return 0.;
    }

private:
    double lb_;
    double ub_;
    bool integer_;
    std::string name_;
};

class MockLinearProblem: public ILinearProblem
{
public:
    std::vector<std::unique_ptr<MockMipVariable>> variables_;

    MockMipVariable* addNumVariable(double lb, double ub, const std::string& name) override
    {
        variables_.emplace_back(std::make_unique<MockMipVariable>(lb, ub, false, name));
        return variables_.back().get();
    }

    MockMipVariable* addIntVariable(double lb, double ub, const std::string& name) override
    {
        variables_.emplace_back(std::make_unique<MockMipVariable>(lb, ub, true, name));
        isLP_ = false;
        return variables_.back().get();
    }

    MockMipVariable* addVariable(double lb,
                                 double ub,
                                 bool integer,
                                 const std::string& name) override
    {
        return integer ? addIntVariable(lb, ub, name) : addNumVariable(lb, ub, name);
    }

    MockMipVariable* getVariable(std::size_t idx) const override
    {
        return variables_[idx].get();
    }

    MockMipVariable* lookupVariable(const std::string& name) const override
    {
        for (const auto& var: variables_)
        {
            if (var->getName() == name)
            {
                return var.get();
            }
        }
        return nullptr;
    }

    int variableCount() const override
    {
        return static_cast<int>(variables_.size());
    }

    IMipConstraint* addConstraint(double, double, const std::string&) override
    {
        return nullptr;
    }

    IMipConstraint* lookupConstraint(const std::string&) const override
    {
        return nullptr;
    }

    IMipConstraint* getConstraint(std::size_t) const override
    {
        return nullptr;
    }

    int constraintCount() const override
    {
        return 0;
    }

    void setObjectiveCoefficient(IMipVariable*, double) override
    {
    }

    double getObjectiveCoefficient(const IMipVariable*) const override
    {
        return 0.0;
    }

    void setMinimization() override
    {
    }

    void setMaximization() override
    {
    }

    bool isMinimization() const override
    {
        return true;
    }

    bool isMaximization() const override
    {
        return false;
    }

    IMipSolution* solve(bool) override
    {
        return nullptr;
    }

    void WriteLP(const std::string&) const override
    {
    }

    double infinity() const override
    {
        return 1e20;
    }

    bool isLP() const override
    {
        return isLP_;
    }

    bool isLP_ = true;
};

BOOST_AUTO_TEST_CASE(Constructor_ValidIndices)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    BOOST_CHECK_NO_THROW(Antares::Optimisation::VariablesBulkAddition(lp, vdict));
}

BOOST_AUTO_TEST_CASE(AddVariable_SingleBounds)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    vba.addVariable(0.0, 1.0, true, dim, key);
    for (int ts = 0; ts < 3; ++ts)
    {
        BOOST_CHECK(vdict("my-component", "my-variable", MCYearAndTime::MCYear{0}, ts) != nullptr);
    }
}

BOOST_AUTO_TEST_CASE(AddVariable_VectorLowerBound)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    std::vector<double> lb = {0.1, 0.2, 0.3};
    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    vba.addVariable(lb, 1.0, true, dim, key);
    for (int ts = 0; ts < 3; ++ts)
    {
        BOOST_CHECK(vdict("my-component", "my-variable", MCYearAndTime::MCYear{0}, ts) != nullptr);
    }
}

BOOST_AUTO_TEST_CASE(AddVariable_VectorUpperBound)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    std::vector<double> ub = {1.1, 1.2, 1.3};
    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    vba.addVariable(0.0, ub, true, dim, key);
    for (int ts = 0; ts < 3; ++ts)
    {
        BOOST_CHECK(vdict("my-component", "my-variable", MCYearAndTime::MCYear{0}, ts) != nullptr);
    }
}

BOOST_AUTO_TEST_CASE(AddVariable_VectorBounds)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    std::vector<double> lb = {0.1, 0.2, 0.3};
    std::vector<double> ub = {1.1, 1.2, 1.3};
    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    vba.addVariable(lb, ub, true, dim, key);
    for (int ts = 0; ts < 3; ++ts)
    {
        BOOST_CHECK(vdict("my-component", "my-variable", MCYearAndTime::MCYear{0}, ts) != nullptr);
    }
}

BOOST_AUTO_TEST_CASE(AddVariable_InvalidBounds)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    std::vector<double> lb = {0.1, 0.2};
    std::vector<double> ub = {1.1, 1.2, 1.3};
    const Dimensions dim({}, IntegerInterval(0, 2));
    const PartialKey key("my-component", "my-variable");
    BOOST_CHECK_THROW(vba.addVariable(lb, ub, true, dim, key), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(AddVariable_InvalidVectorLowerBound)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    BOOST_CHECK_THROW(vba.addVariable({0.1, 0.2}, 1.0, true, dim, key), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(AddVariable_InvalidVectorUpperBound)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);
    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    BOOST_CHECK_THROW(vba.addVariable(0.0, {1.1, 1.2}, true, dim, key), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(AddVariable_InvalidVectorBounds)
{
    MockLinearProblem lp;
    VariableDictionary vdict;
    Antares::Optimisation::VariablesBulkAddition vba(lp, vdict);

    const PartialKey key("my-component", "my-variable");
    const Dimensions dim({}, IntegerInterval(0, 2));
    BOOST_CHECK_THROW(vba.addVariable({0.1, 0.2}, {1.1, 1.2, 1.3}, true, dim, key),
                      std::invalid_argument);
    BOOST_CHECK_THROW(vba.addVariable({0.1, 0.2, 0.3}, {1.1, 1.2}, true, dim, key),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
