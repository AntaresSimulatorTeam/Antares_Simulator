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

#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>

using namespace Antares::Optimisation;

struct FixtureEmptyProblem
{
    FixtureEmptyProblem()
    {
        pb = std::make_unique<LinearProblemMpsolverImpl::OrtoolsLinearProblem>(false, "sirius");
    }

    std::unique_ptr<LinearProblemMpsolverImpl::OrtoolsLinearProblem> pb;
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

// Test bulk addition of integer variables
BOOST_FIXTURE_TEST_CASE(add_int_variables_to_problem___check_all_vars_exist, FixtureEmptyProblem)
{
    auto vars = pb->addIntVariable(1, 5, "int_var", 2);

    BOOST_CHECK_EQUAL(vars.size(), 2); // Check 2 variables created

    for (unsigned int i = 0; i < vars.size(); ++i)
    {
        std::string varName = "int_var_" + std::to_string(i);
        auto* var = pb->getVariable(varName);
        BOOST_CHECK(var);
        BOOST_CHECK(var->isInteger());
        BOOST_CHECK_EQUAL(var->getLb(), 1);
        BOOST_CHECK_EQUAL(var->getUb(), 5);
    }
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

// Test bulk addition of numeric variables
BOOST_FIXTURE_TEST_CASE(add_num_variables_to_problem___check_all_vars_exist, FixtureEmptyProblem)
{
    auto vars = pb->addNumVariable(0, 10, "var", 3);

    BOOST_CHECK_EQUAL(vars.size(), 3); // Check 3 variables created

    for (unsigned int i = 0; i < vars.size(); ++i)
    {
        std::string varName = "var_" + std::to_string(i);
        auto* var = pb->getVariable(varName);
        BOOST_CHECK(var);
        BOOST_CHECK(!var->isInteger());
        BOOST_CHECK_EQUAL(var->getLb(), 0);
        BOOST_CHECK_EQUAL(var->getUb(), 10);
    }
}

// Test bulk addition of variables with mixed types
BOOST_FIXTURE_TEST_CASE(add_mixed_variables_to_problem___check_all_vars_exist, FixtureEmptyProblem)
{
    auto nb_vars_per_type = 2;
    auto num_vars = pb->addVariable(0, 20, false, "num_var", nb_vars_per_type);
    auto int_vars = pb->addVariable(12, 34, true, "int_var", nb_vars_per_type);

    BOOST_CHECK_EQUAL(int_vars.size(), nb_vars_per_type); // Check 2 int variables created
    BOOST_CHECK_EQUAL(num_vars.size(), nb_vars_per_type); // Check 2 numeric variables created

    for (unsigned int i = 0; i < nb_vars_per_type; ++i)
    {
        std::string numVarName = "num_var_" + std::to_string(i);
        std::string intVarName = "int_var_" + std::to_string(i);

        const auto* num_var = pb->getVariable(numVarName);
        const auto* int_var = pb->getVariable(intVarName);
        BOOST_CHECK(num_var);
        BOOST_CHECK(int_var);
        BOOST_CHECK(!num_var->isInteger());
        BOOST_CHECK(int_var->isInteger());
        BOOST_CHECK_EQUAL(num_var->getLb(), 0);
        BOOST_CHECK_EQUAL(num_var->getUb(), 20);
        BOOST_CHECK_EQUAL(int_var->getLb(), 12);
        BOOST_CHECK_EQUAL(int_var->getUb(), 34);
    }
}

BOOST_FIXTURE_TEST_CASE(add_constraint_to_problem___check_constraint_exists, FixtureEmptyProblem)
{
    pb->addConstraint(3., 8., "constraint");
    auto* constraint = pb->getConstraint("constraint");
    BOOST_CHECK(constraint);
    BOOST_CHECK_EQUAL(constraint->getLb(), 3.);
    BOOST_CHECK_EQUAL(constraint->getUb(), 8.);
}

// Test bulk addition of constraints
BOOST_FIXTURE_TEST_CASE(add_constraints_to_problem___check_all_constraints_exist,
                        FixtureEmptyProblem)
{
    auto constraints = pb->addConstraint(5, 15, "constraint", 3);

    BOOST_CHECK_EQUAL(constraints.size(), 3); // Check 3 constraints created

    for (unsigned int i = 0; i < constraints.size(); ++i)
    {
        std::string constraintName = "constraint_" + std::to_string(i);
        auto* constraint = pb->getConstraint(constraintName);
        BOOST_CHECK(constraint);
        BOOST_CHECK_EQUAL(constraint->getLb(), 5);
        BOOST_CHECK_EQUAL(constraint->getUb(), 15);
    }
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

// Test bulk addition of variables with duplicate names
BOOST_FIXTURE_TEST_CASE(add_duplicate_variable_names_leads_to_exception, FixtureEmptyProblem)
{
    pb->addNumVariable(0, 10, "duplicate_var", 1);
    BOOST_CHECK_EXCEPTION(pb->addNumVariable(0, 10, "duplicate_var", 1),
                          std::exception,
                          expectedMessage);
}

BOOST_FIXTURE_TEST_CASE(add_already_existing_constaint_to_problem_leads_to_exception,
                        FixtureEmptyProblem)
{
    pb->addConstraint(0, 1, "constraint");
    BOOST_CHECK_EXCEPTION(pb->addConstraint(0, 1, "constraint"), std::exception, expectedMessage);
}

// Test bulk addition of constraints with duplicate names
BOOST_FIXTURE_TEST_CASE(add_duplicate_constraint_names_leads_to_exception, FixtureEmptyProblem)
{
    pb->addConstraint(0, 1, "duplicate_constraint", 1);
    BOOST_CHECK_EXCEPTION(pb->addConstraint(0, 1, "duplicate_constraint", 1),
                          std::exception,
                          expectedMessage);
}

// Test combining single and bulk addition of variables
BOOST_FIXTURE_TEST_CASE(mix_bulk_and_single_variable_additions, FixtureEmptyProblem)
{
    pb->addNumVariable(0, 5, "single_var");
    auto vars = pb->addNumVariable(5, 10, "bulk_var", 2);

    BOOST_CHECK_EQUAL(pb->variableCount(), 3); // Total variables should be 3

    auto* singleVar = pb->getVariable("single_var");
    BOOST_CHECK(singleVar);
    BOOST_CHECK_EQUAL(singleVar->getLb(), 0);
    BOOST_CHECK_EQUAL(singleVar->getUb(), 5);

    for (unsigned int i = 0; i < vars.size(); ++i)
    {
        std::string varName = "bulk_var_" + std::to_string(i);
        auto* var = pb->getVariable(varName);
        BOOST_CHECK(var);
        BOOST_CHECK_EQUAL(var->getLb(), 5);
        BOOST_CHECK_EQUAL(var->getUb(), 10);
    }
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
    BOOST_CHECK(solution->getStatus() == LinearProblemApi::MipStatus::INFEASIBLE);
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
    BOOST_CHECK(solution->getStatus() == LinearProblemApi::MipStatus::OPTIMAL);
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
