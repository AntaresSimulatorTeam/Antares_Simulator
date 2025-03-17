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
#include <unit_test_utils.h>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/solver/utils/ortools_utils.h"

namespace bdata = boost::unit_test::data;
using SolverType = MPSolver::OptimizationProblemType;

struct SolverTestData
{
    std::string solver_name;
    bool is_mip;
    SolverType expected_type;

    friend std::ostream& operator<<(std::ostream& os, const SolverTestData& data)
    {
        return os << "SolverTestData{solver_name: " << data.solver_name
                  << ", is_mip: " << data.is_mip << ", expected_type: " << data.expected_type
                  << "}";
    }
};

void CheckSolverSupport(const SolverTestData& data)
{
    auto solver = MPSolverFactory(data.is_mip, data.solver_name);
    BOOST_CHECK(solver);
    BOOST_CHECK_EQUAL(data.expected_type, solver->ProblemType());
}

BOOST_AUTO_TEST_SUITE(_ortools_utils_)

// The following solvers support LP & MIP
// TODO : not testing GLPK bc it fails under Windows. OR-Tools probably doesn't activate GLPK by
// default for Windows, even though it does for Linux. Fix this and add tests.
BOOST_DATA_TEST_CASE(
  test_lp_and_mip_solvers_support,
  bdata::make({SolverTestData{"coin", false, SolverType::CLP_LINEAR_PROGRAMMING},
               SolverTestData{"coin", true, SolverType::CBC_MIXED_INTEGER_PROGRAMMING},
               SolverTestData{"highs", false, SolverType::HIGHS_LINEAR_PROGRAMMING},
               SolverTestData{"highs", true, SolverType::HIGHS_MIXED_INTEGER_PROGRAMMING}}))
{
    auto solver = MPSolverFactory(sample.is_mip, sample.solver_name);
    BOOST_REQUIRE(solver);
    BOOST_CHECK_EQUAL(sample.expected_type, solver->ProblemType());
}

// SIRIUS only supports LP
BOOST_AUTO_TEST_CASE(test_sirius_support)
{
    auto lpSolver = MPSolverFactory(false, "sirius");
    BOOST_REQUIRE(lpSolver);
    BOOST_CHECK_EQUAL(SolverType::SIRIUS_LINEAR_PROGRAMMING, lpSolver->ProblemType());
    BOOST_CHECK_EXCEPTION(
      MPSolverFactory(true, "sirius"),
      std::invalid_argument,
      checkMessage("Solver sirius is not supported by Antares or does not support MIP problems."));
}

// PDLP only supports LP
BOOST_AUTO_TEST_CASE(test_pdlp_support)
{
    auto lpSolver = MPSolverFactory(false, "pdlp");
    BOOST_REQUIRE(lpSolver);
    BOOST_CHECK_EQUAL(SolverType::PDLP_LINEAR_PROGRAMMING, lpSolver->ProblemType());
    BOOST_CHECK_EXCEPTION(
      MPSolverFactory(true, "pdlp"),
      std::invalid_argument,
      checkMessage("Solver pdlp is not supported by Antares or does not support MIP problems."));
}

// SCIP only supports MIP
BOOST_AUTO_TEST_CASE(test_scip_support)
{
    BOOST_CHECK_EXCEPTION(
      MPSolverFactory(false, "scip"),
      std::invalid_argument,
      checkMessage("Solver scip is not supported by Antares or does not support LP problems."));
    auto mipSolver = MPSolverFactory(true, "scip");
    BOOST_REQUIRE(mipSolver);
    BOOST_CHECK_EQUAL(SolverType::SCIP_MIXED_INTEGER_PROGRAMMING, mipSolver->ProblemType());
}

BOOST_AUTO_TEST_SUITE_END()
