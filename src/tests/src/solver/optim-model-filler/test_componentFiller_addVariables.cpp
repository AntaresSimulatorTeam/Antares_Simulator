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
#include "antares/modeler-optimisation-container/TimeIndex.h"
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
