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
 * Analysis mock
 */
class AnalysisMock : public UnfeasibilityAnalysis
{
public:
    explicit AnalysisMock(bool shouldDetectCause):
            shouldDetectCause_(shouldDetectCause)
    {}

    ~AnalysisMock() override = default;

    void run(operations_research::MPSolver *problem) override
    {
        hasRun_ = true;
        hasDetectedInfeasibilityCause_ = shouldDetectCause_;
    }

    void printReport() override
    {
        hasPrinted_ = true;
    }

    std::string title() override
    {
        return "mock";
    }

    bool hasRun() const { return hasRun_; }
    bool hasPrinted() const { return hasPrinted_; }

private:
    bool hasRun_ = false;
    bool hasPrinted_ = false;
    bool shouldDetectCause_;
};


BOOST_AUTO_TEST_SUITE(unfeasible_problem_analyzer)

BOOST_AUTO_TEST_CASE(test_problem_analyzer)
{
    auto analysis1 = std::make_shared<AnalysisMock>(false);
    auto analysis2 = std::make_shared<AnalysisMock>(true);
    std::vector<std::shared_ptr<UnfeasibilityAnalysis>> analysis = {
            analysis1,
            analysis2
    };

    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));

    UnfeasiblePbAnalyzer analyzer(analysis);
    BOOST_CHECK(!analysis1->hasRun());
    BOOST_CHECK(!analysis1->hasPrinted());
    BOOST_CHECK(!analysis2->hasRun());
    BOOST_CHECK(!analysis2->hasPrinted());

    analyzer.run(problem.get());
    BOOST_CHECK(analysis1->hasRun());
    BOOST_CHECK(!analysis1->hasPrinted());
    BOOST_CHECK(analysis2->hasRun());
    BOOST_CHECK(!analysis2->hasPrinted());

    // only failing analysis will print
    analyzer.printReport();
    BOOST_CHECK(analysis1->hasRun());
    BOOST_CHECK(!analysis1->hasPrinted());
    BOOST_CHECK(analysis2->hasRun());
    BOOST_CHECK(analysis2->hasPrinted());
}

BOOST_AUTO_TEST_CASE(test_variable_bounds_consistency)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    problem->MakeNumVar(-1, 1, "ok-var");
    problem->MakeNumVar(1, -1, "not-ok-var");

    VariablesBoundsConsistency analysis;
    analysis.run(problem.get());
    auto incorrectVars = analysis.incorrectVars();
    BOOST_CHECK_EQUAL(incorrectVars.size(), 1);

    auto expected = VariableBounds{"not-ok-var", 1, -1};
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
    ConstraintSlackAnalysis analysis;
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    const double inf = problem->infinity();
    auto var1 = problem->MakeNumVar(1, inf, "var1");
    auto var2 = problem->MakeNumVar(-inf, -1, "var2");
    auto constraint = problem->MakeRowConstraint(constraintName);
    constraint->SetBounds(0, inf);
    if (feasible) {
        constraint->SetCoefficient(var1, 1);
        constraint->SetCoefficient(var2, -1);
    } else {
        constraint->SetCoefficient(var1, -1);
        constraint->SetCoefficient(var2, 1);
    }
    return problem;
}

static const std::string validConstraintNames[] =
{
    "BC::hourly::hour<36>",
    "BC::daily::day<67>",
    "BC::weekly::week<12>",
    "FictiveLoads::hour<25>",
    "AreaHydroLevel::hour<8>",
};

BOOST_DATA_TEST_CASE(test_slack_variables_with_unfeasible_constraint, bdata::make(validConstraintNames), constraintName)
{
    std::unique_ptr<MPSolver> unfeasibleProblem = createProblem(constraintName, false);
    BOOST_CHECK(unfeasibleProblem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(unfeasibleProblem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());
}

BOOST_AUTO_TEST_CASE(test_slack_variables_with_unfeasible_constraint_name_ignored)
{
    std::unique_ptr<MPSolver> unfeasibleProblem = createProblem("ignored-name", false);
    BOOST_CHECK(unfeasibleProblem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(unfeasibleProblem.get());
    BOOST_CHECK(!analysis.hasDetectedInfeasibilityCause());
}

BOOST_DATA_TEST_CASE(test_slack_variables_no_problematic_constraint, bdata::make(validConstraintNames), constraintName)
{
    std::unique_ptr<MPSolver> feasibleProblem = createProblem(constraintName, true);
    BOOST_CHECK(feasibleProblem->Solve() == MPSolver::OPTIMAL);

    ConstraintSlackAnalysis analysis;
    analysis.run(feasibleProblem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());  // Would expect false here instead?
}

BOOST_AUTO_TEST_SUITE_END()

