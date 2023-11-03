/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#define WIN32_LEAN_AND_MEAN
#define BOOST_TEST_MODULE unfeasible_problem_analyzer
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/dataset.hpp>

#include "infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "infeasible-problem-analysis/variables-bounds-consistency.h"
#include "infeasible-problem-analysis/constraint-slack-analysis.h"

namespace bdata = boost::unit_test::data;

using operations_research::MPSolver;

using Antares::Optimization::VariablesBoundsConsistency;
using Antares::Optimization::VariableBounds;
using Antares::Optimization::ConstraintSlackAnalysis;
using Antares::Optimization::UnfeasiblePbAnalyzer;
using Antares::Optimization::UnfeasibilityAnalysis;


bool variableEquals(const VariableBounds& lhs, const VariableBounds& rhs)
{
    return lhs.name == rhs.name &&
        lhs.lowBound == rhs.lowBound &&
        lhs.upBound == rhs.upBound;
}

/*!
 * Analysis mock, used to assess which step has been run by the analyzer
 */
class AnalysisMock : public UnfeasibilityAnalysis
{
public:
    AnalysisMock(bool shouldDetectCause, bool& hasRun, bool& hasPrinted):
            shouldDetectCause_(shouldDetectCause),
            hasRun_(hasRun),
            hasPrinted_(hasPrinted)
    {}

    ~AnalysisMock() override = default;

    void run(operations_research::MPSolver *problem) override
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
 *  - Variable 2 must be lesser than -1
 *  - but if feasible is false, constraint enforces that variable 2 is greater than variable 1 --> infeasible
 */
std::unique_ptr<MPSolver> createProblem(const std::string& constraintName, bool feasible)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    const double infinity = problem->infinity();
    auto var1 = problem->MakeNumVar(1, infinity, "var1");
    auto var2 = problem->MakeNumVar(-infinity, -1, "var2");
    auto constraint = problem->MakeRowConstraint(constraintName);
    constraint->SetBounds(0, infinity);
    if (feasible) {
        constraint->SetCoefficient(var1, 1);
        constraint->SetCoefficient(var2, -1);
    } else {
        constraint->SetCoefficient(var1, -1);
        constraint->SetCoefficient(var2, 1);
    }
    return problem;
}

std::unique_ptr<MPSolver> createFeasibleProblem(const std::string& constraintName)
{
    return createProblem(constraintName, true);
}

std::unique_ptr<MPSolver> createUnfeasibleProblem(const std::string& constraintName)
{
    return createProblem(constraintName, false);
}

static const std::string validConstraintNames[] =
{
    "BC::hourly::hour<36>",
    "BC::daily::day<67>",
    "BC::weekly::week<12>",
    "FictiveLoads::hour<25>",
    "AreaHydroLevel::hour<8>",
};

BOOST_DATA_TEST_CASE(analysis_should_detect_unfeasible_constraint,
                     bdata::make(validConstraintNames), constraintName)
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
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());  // Would expect false here instead?
}

BOOST_AUTO_TEST_SUITE_END()

