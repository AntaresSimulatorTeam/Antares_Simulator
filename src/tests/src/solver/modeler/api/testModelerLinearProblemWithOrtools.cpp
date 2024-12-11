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

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/ortoolsImpl/linearProblem.h>

using namespace Antares::Solver::Modeler;

struct FixtureEmptyProblem
{
    FixtureEmptyProblem()
    {
        pb = std::make_unique<OrtoolsImpl::OrtoolsLinearProblem>(false, "sirius");
    }

    std::unique_ptr<OrtoolsImpl::OrtoolsLinearProblem> pb;
};

struct FixtureInfeasibleProblem: public FixtureEmptyProblem
{
    FixtureInfeasibleProblem()
    {
        auto* var = pb->addNumVariable(0, 1, "var");
        auto* constraint = pb->addConstraint(2, 2, "constraint");
        constraint->setCoefficient(var, 1);
    }
};

struct FixtureFeasibleProblem: public FixtureEmptyProblem
{
    FixtureFeasibleProblem()
    {
        auto* var = pb->addNumVariable(0, 10, "var");
        auto* constraint = pb->addConstraint(1, 1, "constraint");
        constraint->setCoefficient(var, 1);
        pb->setObjectiveCoefficient(var, 1);
    }
};

BOOST_AUTO_TEST_SUITE(tests_on_OrtoolsLinearProblem)

BOOST_FIXTURE_TEST_CASE(add_int_variable_to_problem___check_var_exists, FixtureEmptyProblem)
{
    pb->addIntVariable(5, 15, "var");
    auto* var = pb->getVariable("var");
    BOOST_CHECK(var);
    BOOST_CHECK(var->isInteger());
    BOOST_CHECK_EQUAL(var->getLb(), 5);
    BOOST_CHECK_EQUAL(var->getUb(), 15);
}

BOOST_FIXTURE_TEST_CASE(add_num_variable_to_problem___check_var_exists, FixtureEmptyProblem)
{
    pb->addNumVariable(2., 7., "var");
    auto* var = pb->getVariable("var");
    BOOST_CHECK(var);
    BOOST_CHECK(!var->isInteger());
    BOOST_CHECK_EQUAL(var->getLb(), 2.);
    BOOST_CHECK_EQUAL(var->getUb(), 7.);
}

BOOST_FIXTURE_TEST_CASE(add_constraint_to_problem___check_constraint_exists, FixtureEmptyProblem)
{
    pb->addConstraint(3., 8., "constraint");
    auto* constraint = pb->getConstraint("constraint");
    BOOST_CHECK(constraint);
    BOOST_CHECK_EQUAL(constraint->getLb(), 3.);
    BOOST_CHECK_EQUAL(constraint->getUb(), 8.);
}

BOOST_FIXTURE_TEST_CASE(give_coeff_to_var_in_constraint____check_coeff_exists, FixtureEmptyProblem)
{
    auto* var = pb->addNumVariable(0, 1, "var");
    auto* constraint = pb->addConstraint(0, 1, "constraint");
    constraint->setCoefficient(var, 3.2);

    BOOST_CHECK_EQUAL(constraint->getCoefficient(var), 3.2);
}

BOOST_FIXTURE_TEST_CASE(give_coef_to_null_var_in_constaint_leads_to_bad_cast, FixtureEmptyProblem)
{
    auto* constraint = pb->addConstraint(0, 1, "constraint");
    BOOST_CHECK_THROW(constraint->setCoefficient(nullptr, 3.2), std::bad_cast);
}

BOOST_FIXTURE_TEST_CASE(get_coef_of_null_var_in_constaint_leads_to_bad_cast, FixtureEmptyProblem)
{
    auto* constraint = pb->addConstraint(0, 1, "constraint");
    BOOST_CHECK_THROW(constraint->getCoefficient(nullptr), std::bad_cast);
}

bool expectedMessage(const std::exception& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), std::string("Element name already exists in linear problem"));
    return true;
}

BOOST_FIXTURE_TEST_CASE(add_already_existing_var_to_problem_leads_to_exception, FixtureEmptyProblem)
{
    pb->addNumVariable(0, 1, "var");
    BOOST_CHECK_EXCEPTION(pb->addNumVariable(0, 1, "var"), std::exception, expectedMessage);
}

BOOST_FIXTURE_TEST_CASE(add_already_existing_constaint_to_problem_leads_to_exception,
                        FixtureEmptyProblem)
{
    pb->addConstraint(0, 1, "constraint");
    BOOST_CHECK_EXCEPTION(pb->addConstraint(0, 1, "constraint"), std::exception, expectedMessage);
}

BOOST_FIXTURE_TEST_CASE(minimize_problem___check_minimize_status, FixtureEmptyProblem)
{
    pb->setMinimization();
    BOOST_CHECK(pb->isMinimization());
}

BOOST_FIXTURE_TEST_CASE(maximize_problem___check_maximize_status, FixtureEmptyProblem)
{
    pb->setMaximization();
    BOOST_CHECK(pb->isMaximization());
}

BOOST_FIXTURE_TEST_CASE(give_bounds_to_var___check_bounds_exist, FixtureEmptyProblem)
{
    auto* var = pb->addNumVariable(0, 1, "var");
    var->setLb(-4);
    var->setUb(7);

    BOOST_CHECK_EQUAL(var->getLb(), -4);
    BOOST_CHECK_EQUAL(var->getUb(), 7);

    var->setBounds(2, 13);

    BOOST_CHECK_EQUAL(var->getLb(), 2);
    BOOST_CHECK_EQUAL(var->getUb(), 13);
}

BOOST_FIXTURE_TEST_CASE(give_bounds_to_constraint___check_bounds_exist, FixtureEmptyProblem)
{
    auto* constraint = pb->addConstraint(0, 1, "var");

    constraint->setLb(-4);
    constraint->setUb(7);
    BOOST_CHECK_EQUAL(constraint->getLb(), -4);
    BOOST_CHECK_EQUAL(constraint->getUb(), 7);

    constraint->setBounds(2, 13);
    BOOST_CHECK_EQUAL(constraint->getLb(), 2);
    BOOST_CHECK_EQUAL(constraint->getUb(), 13);
}

BOOST_FIXTURE_TEST_CASE(give_cost_to_variable___check_cost_exist, FixtureEmptyProblem)
{
    auto* var = pb->addIntVariable(0, 1, "var");
    pb->setObjectiveCoefficient(var, 1);
    BOOST_CHECK_EQUAL(pb->getObjectiveCoefficient(var), 1);
}

BOOST_FIXTURE_TEST_CASE(get_cost_from_null_variable_leads_to_bad_cast, FixtureEmptyProblem)
{
    BOOST_CHECK_THROW(pb->getObjectiveCoefficient(nullptr), std::bad_cast);
}

BOOST_FIXTURE_TEST_CASE(give_cost_to_null_variable_leads_to_bad_cast, FixtureEmptyProblem)
{
    BOOST_CHECK_THROW(pb->setObjectiveCoefficient(nullptr, 0), std::bad_cast);
}

BOOST_FIXTURE_TEST_CASE(solve_infeasible_problem_leads_to_error_status, FixtureInfeasibleProblem)
{
    auto* solution = pb->solve(true);
    BOOST_CHECK(solution->getStatus() == Api::MipStatus::INFEASIBLE);
}

BOOST_FIXTURE_TEST_CASE(solve_infeasible_problem_leads_to_null_objective_value,
                        FixtureInfeasibleProblem)
{
    auto* solution = pb->solve(true);
    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 0);
}

BOOST_FIXTURE_TEST_CASE(solve_infeasible_problem___check_any_var_is_zero, FixtureInfeasibleProblem)
{
    auto* solution = pb->solve(true);

    auto* var = pb->getVariable("var");
    BOOST_CHECK(var); // searched variable is known by problem
    BOOST_CHECK_EQUAL(solution->getOptimalValue(var), 0);
}

BOOST_FIXTURE_TEST_CASE(solve_feasible_problem___check_status_is_optimal, FixtureFeasibleProblem)
{
    auto* solution = pb->solve(false);
    BOOST_CHECK(solution->getStatus() == Api::MipStatus::OPTIMAL);
}

BOOST_FIXTURE_TEST_CASE(solve_feasible_problem___check_objective_has_expected_value,
                        FixtureFeasibleProblem)
{
    auto* solution = pb->solve(false);
    BOOST_CHECK_EQUAL(solution->getObjectiveValue(), 1);
}

BOOST_FIXTURE_TEST_CASE(solve_problem_then_add_new_var___new_var_optimal_value_is_zero,
                        FixtureFeasibleProblem)
{
    auto* solution = pb->solve(false);
    auto* newVar = pb->addNumVariable(0, 1, "new var");
    BOOST_CHECK_EQUAL(solution->getOptimalValue(newVar), 0);
}

BOOST_FIXTURE_TEST_CASE(solve_problem___check_optimal_value_of_null_var_is_zero,
                        FixtureFeasibleProblem)
{
    auto* solution = pb->solve(false);
    BOOST_CHECK_EQUAL(solution->getOptimalValue(nullptr), 0);
}

BOOST_AUTO_TEST_SUITE_END()
