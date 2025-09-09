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

#include <antares/optimisation/linear-problem-api/linearProblemBuilder.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>

#include "mock-fillers/FillerContext.h"
#include "mock-fillers/OneConstraintFiller.h"
#include "mock-fillers/OneVarFiller.h"
#include "mock-fillers/TwoVarsTwoConstraintsFiller.h"

using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;

struct Fixture
{
    Fixture()
    {
        pb = std::make_unique<OrtoolsLinearProblem>(false, "sirius");
    }

    std::vector<std::unique_ptr<LinearProblemFiller>> fillers;
    LinearProblemData LP_Data;
    FillContext ctx = {0, 0, 0, 0, 0}; // dummy value for other tests than context
    std::unique_ptr<ILinearProblem> pb;
};

BOOST_AUTO_TEST_SUITE(tests_on_linear_problem_builder)

BOOST_FIXTURE_TEST_CASE(no_filler_given_to_builder___nothing_built, Fixture)
{
    LinearProblemBuilder lpBuilder(fillers);
    lpBuilder.build(*pb, LP_Data, ctx);

    BOOST_CHECK_EQUAL(pb->variableCount(), 0);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
}

BOOST_FIXTURE_TEST_CASE(one_var_filler___the_var_is_built, Fixture)
{
    fillers.push_back(std::make_unique<OneVarFiller>());

    LinearProblemBuilder lpBuilder(fillers);
    lpBuilder.build(*pb, LP_Data, ctx);

    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 0);
    auto* var = pb->lookupVariable("var-by-OneVarFiller");
    BOOST_CHECK(var);
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 1);
}

BOOST_FIXTURE_TEST_CASE(one_constraint_filler___the_constraint_is_built, Fixture)
{
    fillers.push_back(std::make_unique<OneConstraintFiller>());

    LinearProblemBuilder lpBuilder(fillers);
    lpBuilder.build(*pb, LP_Data, ctx);

    BOOST_CHECK_EQUAL(pb->variableCount(), 0);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    BOOST_CHECK(pb->lookupConstraint("constraint-by-OneConstraintFiller"));
}

BOOST_FIXTURE_TEST_CASE(two_fillers_given_to_builder___all_is_built, Fixture)
{
    fillers.push_back(std::make_unique<OneVarFiller>());
    fillers.push_back(std::make_unique<OneConstraintFiller>());

    LinearProblemBuilder lpBuilder(fillers);
    lpBuilder.build(*pb, LP_Data, ctx);

    BOOST_CHECK_EQUAL(pb->constraintCount(), 1);
    BOOST_CHECK(pb->lookupConstraint("constraint-by-OneConstraintFiller"));
    BOOST_CHECK_EQUAL(pb->variableCount(), 1);
}

BOOST_FIXTURE_TEST_CASE(three_fillers_given_to_builder___3_vars_3_constr_are_built, Fixture)
{
    fillers.push_back(std::make_unique<OneVarFiller>());
    fillers.push_back(std::make_unique<OneConstraintFiller>());
    fillers.push_back(std::make_unique<TwoVarsTwoConstraintsFiller>());

    LinearProblemBuilder lpBuilder(fillers);
    lpBuilder.build(*pb, LP_Data, ctx);

    BOOST_CHECK_EQUAL(pb->variableCount(), 3);
    BOOST_CHECK_EQUAL(pb->constraintCount(), 3);
}

BOOST_FIXTURE_TEST_CASE(FillerWithContext, Fixture)
{
    fillers.push_back(std::make_unique<VarFillerContext>());

    ctx = FillContext(0, 5, 0, 5, 0);

    ctx.addSelectedScenarios(0);
    ctx.addSelectedScenarios(2);

    LinearProblemBuilder lpBuilder(fillers);
    lpBuilder.build(*pb, LP_Data, ctx);

    BOOST_CHECK_EQUAL(pb->variableCount(), 10); // 5 timestep * 2 scenario

    auto var1 = pb->lookupVariable("variable-ts0-sc0");
    BOOST_CHECK_EQUAL(var1->getLb(), 1);

    auto var2 = pb->lookupVariable("variable-ts3-sc2");
    BOOST_CHECK_EQUAL(var2->getLb(), 12);
}

BOOST_AUTO_TEST_SUITE_END()
