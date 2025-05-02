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

#include <cmath>
#include <pi_constantes_externes.h>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/utils/ortools_quadratic_wrapper.h>

#include "spx_constantes_externes.h"
static double tolerance = 1e-5;

struct QpFixture
{
    QpFixture()
    {
        // We only support PDLP solver for now
        options.solverName = "pdlp";
        // Init empty problem
        problemeAResoudre.NombreDeVariables = 0;
        problemeAResoudre.NombreDeContraintes = 0;
        problemeAResoudre.IncrementDAllocationMatriceDesContraintes = 0;
        problemeAResoudre.NombreDeTermesDansLaMatriceDesContraintes = 0;
        problemeAResoudre.ExistenceDUneSolution = NON_PI;
    }

    PROBLEME_ANTARES_A_RESOUDRE problemeAResoudre;
    std::vector<double> primals;
    std::vector<double> duals;
    std::vector<double> reducedCosts;
    Antares::Solver::Optimization::SingleOptimOptions options;

    void solve()
    {
        problemeAResoudre.X.resize(problemeAResoudre.NombreDeVariables);
        primals.resize(problemeAResoudre.NombreDeVariables);
        problemeAResoudre.CoutsMarginauxDesContraintes.resize(
          problemeAResoudre.NombreDeContraintes);
        duals.resize(problemeAResoudre.NombreDeContraintes);
        problemeAResoudre.CoutsReduits.resize(problemeAResoudre.NombreDeVariables);
        reducedCosts.resize(problemeAResoudre.NombreDeVariables);
        for (int i = 0; i < problemeAResoudre.NombreDeVariables; ++i)
        {
            problemeAResoudre.AdresseOuPlacerLaValeurDesVariablesOptimisees.emplace_back(
              &primals[i]);
            problemeAResoudre.AdresseOuPlacerLaValeurDesCoutsReduits.emplace_back(&reducedCosts[i]);
        }
        for (int i = 0; i < problemeAResoudre.NombreDeContraintes; ++i)
        {
            problemeAResoudre.AdresseOuPlacerLaValeurDesCoutsMarginaux.emplace_back(&duals[i]);
        }
        SolveQuadraticProblemWithOrtools(options, &problemeAResoudre);
    }

    void addVar(const std::string& name, double lb, double ub, double linObj, double quadObj)
    {
        ++problemeAResoudre.NombreDeVariables;
        problemeAResoudre.NomDesVariables.emplace_back(name);
        problemeAResoudre.VariablesEntieres.emplace_back(false);
        problemeAResoudre.Xmin.emplace_back(lb);
        problemeAResoudre.Xmax.emplace_back(ub);
        problemeAResoudre.CoutQuadratique.emplace_back(quadObj);
        problemeAResoudre.CoutLineaire.emplace_back(linObj);
        int type = VARIABLE_NON_BORNEE;
        if (std::isfinite(lb) && std::isfinite(ub))
        {
            type = ub - lb < 1e-3 ? VARIABLE_FIXE : VARIABLE_BORNEE_DES_DEUX_COTES;
        }
        else if (std::isfinite(lb))
        {
            type = VARIABLE_BORNEE_INFERIEUREMENT;
        }
        else if (std::isfinite(ub))
        {
            type = VARIABLE_BORNEE_SUPERIEUREMENT;
        }
        problemeAResoudre.TypeDeVariable.emplace_back(type);
    }

    void addConstr(const std::string& name,
                   double lb,
                   double ub,
                   const std::vector<int>& vars,
                   const std::vector<double>& coefs)
    {
        ++problemeAResoudre.NombreDeContraintes;
        problemeAResoudre.NomDesContraintes.emplace_back(name);
        std::string sens;
        if (lb == ub)
        {
            problemeAResoudre.Sens += "=";
            problemeAResoudre.SecondMembre.emplace_back(lb);
        }
        else if (std::isinf(lb))
        {
            problemeAResoudre.Sens += "<";
            problemeAResoudre.SecondMembre.emplace_back(ub);
        }
        else if (std::isinf(ub))
        {
            problemeAResoudre.Sens += ">";
            problemeAResoudre.SecondMembre.emplace_back(lb);
        }
        else
        {
            throw std::invalid_argument(
              "Constraint lb and ub must be equal, or one of them infinite");
        }
        int iStart = problemeAResoudre.CoefficientsDeLaMatriceDesContraintes.size();
        problemeAResoudre.IndicesDebutDeLigne.emplace_back(iStart);
        problemeAResoudre.NombreDeTermesDesLignes.emplace_back(coefs.size());
        for (unsigned i = 0; i < coefs.size(); ++i)
        {
            problemeAResoudre.IndicesColonnes[iStart + i] = vars[i];
            problemeAResoudre.CoefficientsDeLaMatriceDesContraintes[iStart + i] = coefs[i];
        }
    }

    void checkPrimalSolution(const std::vector<double>& expected)
    {
        BOOST_TEST(problemeAResoudre.X == expected,
                   boost::test_tools::tolerance(tolerance)
                     << "comparison failed" << boost::test_tools::per_element());
        BOOST_TEST(primals == expected,
                   boost::test_tools::tolerance(tolerance)
                     << "comparison failed" << boost::test_tools::per_element());
    }

    void checkDualSolution(const std::vector<double>& expected)
    {
        BOOST_TEST(problemeAResoudre.CoutsMarginauxDesContraintes == expected,
                   boost::test_tools::tolerance(tolerance)
                     << "comparison failed" << boost::test_tools::per_element());
        BOOST_TEST(duals == expected,
                   boost::test_tools::tolerance(tolerance)
                     << "comparison failed" << boost::test_tools::per_element());
    }

    void checkReducedCosts(const std::vector<double>& expected)
    {
        BOOST_TEST(problemeAResoudre.CoutsReduits == expected,
                   boost::test_tools::tolerance(tolerance)
                     << "comparison failed" << boost::test_tools::per_element());
        BOOST_TEST(reducedCosts == expected,
                   boost::test_tools::tolerance(tolerance)
                     << "comparison failed" << boost::test_tools::per_element());
    }

    void checkAllNan(const std::vector<double>& actual)
    {
        for (unsigned i = 0; i < actual.size(); ++i)
        {
            BOOST_TEST(std::isnan(actual[i]));
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(tests_on_ortools_quadratic_wrapper, QpFixture)

BOOST_AUTO_TEST_CASE(solver_not_supported)
{
    options.solverName = "sirius";
    BOOST_CHECK_EXCEPTION(
      solve(),
      std::invalid_argument,
      checkMessage(
        "Solver sirius is not supported for quadratic problems optimization through MathOpt."));
}

BOOST_AUTO_TEST_CASE(simple_qp_one_var)
{
    // minimize(x * x - 0.5 * x)
    // such that 0 <= x <= 1
    // expected optimal value of x : 0.25
    addVar("x", 0, 1, -0.5, 1);
    solve();
    BOOST_CHECK_EQUAL(OUI_PI, problemeAResoudre.ExistenceDUneSolution);
    checkPrimalSolution({0.25});
    checkDualSolution({});
    checkReducedCosts({0});
}

BOOST_AUTO_TEST_CASE(simple_qp_one_var_fixed)
{
    // minimize(x * x - 0.5 * x)
    // such that 0.24 <= x <= 0.2408
    // although optimal value of x would be 0.25, the wrapper's handling of case VARIABLE_FIXE
    // would set lb = ub = 0.5 * (0.24 + 0.2408) = 0.2404
    addVar("x", 0.24, 0.2408, -0.5, 1);
    solve();
    BOOST_CHECK_EQUAL(OUI_PI, problemeAResoudre.ExistenceDUneSolution);
    checkPrimalSolution({0.2404});
}

BOOST_AUTO_TEST_CASE(simple_qp_two_vars_1)
{
    // Primal:
    //   min  2x_0^2 + 0.5x_1^2 - x_0 - x_1 + 5
    //   s.t. -inf <= x_0 + x_1 <= 1
    //         1 <= x_0 <= 2
    //        -2 <= x_1 <= 4
    //
    // Optimal solution: x* = (1, 0).
    //
    // Dual :
    //   max  -2x_0^2 - 0.5x_1^2 + y_0 + min{r_0, 2r_0} + min{-2r_1, 4r_1} + 5
    //   s.t. y_0 + r_0 = 4x_0 - 1
    //        y_0 + r_1 = x_1 - 1
    //        y_0 <= 0
    //
    //  Optimal solution: x* = (1, 0), y* = (-1), r* = (4, 0).
    addVar("x0", 1, 2, -1, 2);
    addVar("x1", -2, 4, -1, 0.5);
    addConstr("c1", -std::numeric_limits<double>::infinity(), 1, {0, 1}, {1, 1});
    solve();
    BOOST_CHECK_EQUAL(OUI_PI, problemeAResoudre.ExistenceDUneSolution);
    checkPrimalSolution({1, 0});
    checkDualSolution({-1});
    checkReducedCosts({4, 0});
}

BOOST_AUTO_TEST_CASE(simple_qp_two_vars_2)
{
    // Primal:
    //   min  0.5x_0^2 + 0.5x_1^2 - 3x_0 - x_1
    //   s.t. 2 <= x_0 - x_1 <= 2
    //        0 <= x_0 <= inf
    //        0 <= x_1 <= inf
    //
    // Optimal solution: x* = (3, 1).
    //
    // Dual (go/mathopt-qp-dual):
    //   max  -0.5x_0^2 - 0.5x_1^2 + 2y_0
    //   s.t.  y_0 + r_0 = x_0 - 3
    //        -y_0 + r_1 = x_1 - 1
    //        r_0 >= 0
    //        r_1 >= 0
    //
    //  Optimal solution: x* = (3, 1), y* = (0), r* = (0, 0).
    addVar("x0", 0, std::numeric_limits<double>::infinity(), -3, 0.5);
    addVar("x1", 0, std::numeric_limits<double>::infinity(), -1, 0.5);
    addConstr("c1", 2, 2, {0, 1}, {1, -1});
    solve();
    BOOST_CHECK_EQUAL(OUI_PI, problemeAResoudre.ExistenceDUneSolution);
    checkPrimalSolution({3, 1});
    checkDualSolution({0});
    checkReducedCosts({0, 0});
}

BOOST_AUTO_TEST_CASE(infeasible_qp)
{
    // minimize(x * x - 0.5 * x)
    // such that -inf <= x <= 1
    //       and      x >= 3
    addVar("x", -std::numeric_limits<double>::infinity(), 1, -0.5, 1);
    addConstr("c1", 3, std::numeric_limits<double>::infinity(), {0}, {1});
    solve();
    BOOST_CHECK_EQUAL(NON_PI, problemeAResoudre.ExistenceDUneSolution);
    checkAllNan(problemeAResoudre.X);
    checkAllNan(primals);
    checkAllNan(problemeAResoudre.CoutsMarginauxDesContraintes);
    checkAllNan(duals);
    checkAllNan(problemeAResoudre.CoutsReduits);
    checkAllNan(reducedCosts);
}

BOOST_AUTO_TEST_CASE(unbounded_qp)
{
    // minimize(x)
    // such that -inf <= x <= inf
    addVar("x",
           -std::numeric_limits<double>::infinity(),
           std::numeric_limits<double>::infinity(),
           1,
           0);
    solve();
    BOOST_CHECK_EQUAL(NON_PI, problemeAResoudre.ExistenceDUneSolution);
    checkAllNan(problemeAResoudre.X);
    checkAllNan(primals);
    checkAllNan(problemeAResoudre.CoutsMarginauxDesContraintes);
    checkAllNan(duals);
    checkAllNan(problemeAResoudre.CoutsReduits);
    checkAllNan(reducedCosts);
}

BOOST_AUTO_TEST_CASE(invalid_variable_type)
{
    addVar("x", -std::numeric_limits<double>::infinity(), 1, -0.5, 1);
    problemeAResoudre.TypeDeVariable[0] = 15;
    BOOST_CHECK_EXCEPTION(solve(),
                          std::invalid_argument,
                          checkMessage("Unknown variable type: 15"));
}

BOOST_AUTO_TEST_CASE(invalid_constraint_sense)
{
    addVar("x", -std::numeric_limits<double>::infinity(), 1, -0.5, 1);
    addConstr("c1", 3, std::numeric_limits<double>::infinity(), {0}, {1});
    problemeAResoudre.Sens = "+";
    BOOST_CHECK_EXCEPTION(solve(),
                          std::invalid_argument,
                          checkMessage("Expected constraint sense to be =, <, or >, but was: +"));
}

BOOST_AUTO_TEST_SUITE_END()
