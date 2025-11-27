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
#include "antares/modeler-optimisation-container/VariabilityType.h"
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
