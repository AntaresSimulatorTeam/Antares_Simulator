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
#define BOOST_TEST_MODULE unfeasible_problem_analyzer

#include <ortools/linear_solver/linear_solver.h>

#include <boost/test/data/dataset.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/solver/infeasible-problem-analysis/constraint-slack-analysis.h"
#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "antares/solver/infeasible-problem-analysis/variables-bounds-consistency.h"

namespace bdata = boost::unit_test::data;

using namespace operations_research;

using Antares::Optimization::ConstraintSlackAnalysis;
using Antares::Optimization::UnfeasibilityAnalysis;
using Antares::Optimization::UnfeasiblePbAnalyzer;
using Antares::Optimization::VariableBounds;
using Antares::Optimization::VariablesBoundsConsistency;

bool variableEquals(const VariableBounds& lhs, const VariableBounds& rhs)
{
    return lhs.name == rhs.name && lhs.lowBound == rhs.lowBound && lhs.upBound == rhs.upBound;
}

/*!
 * Analysis mock, used to assess which step has been run by the analyzer
 */
class AnalysisMock: public UnfeasibilityAnalysis
{
public:
    AnalysisMock(bool shouldDetectCause, bool& hasRun, bool& hasPrinted):
        shouldDetectCause_(shouldDetectCause),
        hasRun_(hasRun),
        hasPrinted_(hasPrinted)
    {
    }

    ~AnalysisMock() override = default;

    void run(operations_research::MPSolver*) override
    {
        hasRun_ = true;
        hasDetectedInfeasibilityCause_ = shouldDetectCause_;
    }

    void printReport() const override
    {
        hasPrinted_ = true;
    }

    std::string title() const override
    {
        return "mock";
    }

private:
    bool shouldDetectCause_;
    bool& hasRun_;
    bool& hasPrinted_;
};

BOOST_AUTO_TEST_SUITE(unfeasible_problem_analyzer)

BOOST_AUTO_TEST_CASE(analyzer_should_call_analysis_and_print_detected_issues)
{
    bool hasRun1 = false;
    bool hasPrinted1 = false;
    bool hasRun2 = false;
    bool hasPrinted2 = false;
    // Vector is moved, iterators are invalidated. Can't hold references to element to get values.
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysis;
    analysis.push_back(std::make_unique<AnalysisMock>(false, hasRun1, hasPrinted1));
    analysis.push_back(std::make_unique<AnalysisMock>(true, hasRun2, hasPrinted2));
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));

    UnfeasiblePbAnalyzer analyzer(std::move(analysis));
    BOOST_CHECK(!hasRun1);
    BOOST_CHECK(!hasPrinted1);
    BOOST_CHECK(!hasRun2);
    BOOST_CHECK(!hasPrinted2);

    analyzer.run(problem.get());
    BOOST_CHECK(hasRun1);
    BOOST_CHECK(!hasPrinted1);
    BOOST_CHECK(hasRun2);
    BOOST_CHECK(!hasPrinted2);

    // only failing analysis will print
    analyzer.printReport();
    BOOST_CHECK(hasRun1);
    BOOST_CHECK(!hasPrinted1);
    BOOST_CHECK(hasRun2);
    BOOST_CHECK(hasPrinted2);
}

BOOST_AUTO_TEST_CASE(analysis_should_detect_inconsistent_variable_bounds)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    problem->MakeNumVar(-1, 1, "ok-var");
    problem->MakeNumVar(1, -1, "not-ok-var");

    VariablesBoundsConsistency analysis;
    analysis.run(problem.get());
    auto incorrectVars = analysis.incorrectVars();
    BOOST_CHECK_EQUAL(incorrectVars.size(), 1);

    auto expected = VariableBounds("not-ok-var", 1, -1);
    BOOST_CHECK(variableEquals(incorrectVars[0], expected));
}

/*!
 * Creates a problem with 2 variables linked by 1 constraint:
 *  - Variable 1 must be greater than 1
 *  - Variable 2 must be smaller than -1
 *  - but if feasible is false, constraint enforces that variable 2 is greater than variable 1 -->
 * infeasible
 */

std::unique_ptr<MPSolver> createProblem(const std::string& constraintName)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    const double infinity = problem->infinity();
    auto constraint = problem->MakeRowConstraint(constraintName);
    constraint->SetBounds(0, infinity);
    return problem;
}

std::unique_ptr<MPSolver> createFeasibleProblem(const std::string& constraintName)
{
    auto problem = createProblem(constraintName);
    auto constraint = problem->LookupConstraintOrNull(constraintName);
    auto var1 = problem->LookupVariableOrNull("var1");
    auto var2 = problem->LookupVariableOrNull("var2");
    constraint->SetCoefficient(var1, 1);
    constraint->SetCoefficient(var2, -1);
    return problem;
}

std::unique_ptr<MPSolver> createUnfeasibleProblem(const std::string& constraintName)
{
    auto problem = createProblem(constraintName);
    auto constraint = problem->LookupConstraintOrNull(constraintName);
    auto var1 = problem->LookupVariableOrNull("var1");
    auto var2 = problem->LookupVariableOrNull("var2");
    constraint->SetCoefficient(var1, -1);
    constraint->SetCoefficient(var2, 1);
    return problem;
}

static const std::string validConstraintNames[] = {"BC-name-1::hourly::hour<36>",
                                                   "BC-name-2::daily::day<67>",
                                                   "BC-name-3::weekly::week<12>",
                                                   "FictiveLoads::area<some-area>::hour<25>",
                                                   "AreaHydroLevel::area<some-area>::hour<8>",
                                                   "Level::area<some-area>::hour<28>",
                                                   "HydroPower::area<some-area>::week<45>"};

BOOST_DATA_TEST_CASE(analysis_should_detect_unfeasible_constraint,
                     bdata::make(validConstraintNames),
                     constraintName)
{
    std::unique_ptr<MPSolver> unfeasibleProblem = createUnfeasibleProblem(constraintName);
    BOOST_CHECK(unfeasibleProblem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(unfeasibleProblem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());
}

BOOST_AUTO_TEST_CASE(analysis_should_ignore_ill_named_constraint)
{
    std::unique_ptr<MPSolver> unfeasibleProblem = createUnfeasibleProblem("ignored-name");
    BOOST_CHECK(unfeasibleProblem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(unfeasibleProblem.get());
    BOOST_CHECK(!analysis.hasDetectedInfeasibilityCause());
}

// TODO: this test should be improved by changing the API, the current interface does not allow
//       to check that no constraint was identified...
BOOST_AUTO_TEST_CASE(analysis_should_ignore_feasible_constraints)
{
    std::unique_ptr<MPSolver> feasibleProblem = createFeasibleProblem("BC::hourly::hour<36>");
    BOOST_CHECK(feasibleProblem->Solve() == MPSolver::OPTIMAL);

    ConstraintSlackAnalysis analysis;
    analysis.run(feasibleProblem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause()); // Would expect false here instead?
}

BOOST_AUTO_TEST_SUITE_END()
