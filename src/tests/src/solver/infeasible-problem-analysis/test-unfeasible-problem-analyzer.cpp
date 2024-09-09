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
#include <ranges>

#include <boost/test/data/dataset.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/solver/infeasible-problem-analysis/constraint-slack-analysis.h"
#include "antares/solver/infeasible-problem-analysis/report.h"
#include "antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h"
#include "antares/solver/infeasible-problem-analysis/variables-bounds-consistency.h"

namespace bdata = boost::unit_test::data;

using namespace operations_research;

using Antares::Optimization::ConstraintSlackAnalysis;
using Antares::Optimization::InfeasibleProblemReport;
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

void addOneVarConstraintToProblem(MPSolver* problem,
                                  const std::string& constraintName,
                                  double varLowBnd,
                                  double varUpBnd,
                                  double ConstLowBnd)
{
    std::string varName = "lonely-var-in-" + constraintName;
    auto* var = problem->MakeNumVar(varLowBnd, varUpBnd, varName);
    auto* constraint = problem->MakeRowConstraint(constraintName);
    constraint->SetCoefficient(var, 1);
    constraint->SetBounds(ConstLowBnd, problem->infinity());
}

std::unique_ptr<MPSolver> createFeasibleProblem(const std::string& constraintName)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    // Following constraint is easily satisfied
    addOneVarConstraintToProblem(problem.get(), constraintName, 0., 1., 0.);
    return problem;
}

std::unique_ptr<MPSolver> createUnfeasibleProblem(const std::string& constraintName)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    // Following constraint cannot be satisfied
    addOneVarConstraintToProblem(problem.get(), constraintName, 0., 1., 2.);
    return problem;
}

std::unique_ptr<MPSolver> createProblemWith_n_violatedConstraints(const int n)
{
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));
    for (auto i: std::ranges::iota_view(1, n + 1)) // From 1 to n included
    {
        std::stringstream name;
        name << "BC-name-" << i << "::hourly::hour<" << 5 * i << ">";
        // Make a constraint that can never be satisfied, of type : var > A,
        // where : bound_inf(var) = 0, bound_sup(var) = 1 and A > 1.
        addOneVarConstraintToProblem(problem.get(), name.str(), 0., 1., double(i + 2));
    }
    return problem;
}

BOOST_AUTO_TEST_SUITE(general_unfeasible_problem_analyzer)

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
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(variable_bounds_consistency_analyzer)

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
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(slack_variables_analyzer)

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

BOOST_AUTO_TEST_CASE(analysis_should_ignore_feasible_constraints)
{
    std::unique_ptr<MPSolver> feasibleProblem = createFeasibleProblem("BC-name::hourly::hour<36>");
    BOOST_CHECK(feasibleProblem->Solve() == MPSolver::OPTIMAL);

    ConstraintSlackAnalysis analysis;
    analysis.run(feasibleProblem.get());
    BOOST_CHECK(!analysis.hasDetectedInfeasibilityCause());
}

BOOST_AUTO_TEST_CASE(analysis_slack_variables_are_ordered)
{
    std::unique_ptr<MPSolver> problem = createProblemWith_n_violatedConstraints(3);
    BOOST_CHECK(problem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(problem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());

    auto& violatedConstraints = analysis.largestSlackVariables();
    BOOST_CHECK_EQUAL(violatedConstraints.size(), 3);
    BOOST_CHECK_EQUAL(violatedConstraints[0]->name(), "BC-name-3::hourly::hour<15>::low");
    BOOST_CHECK_EQUAL(violatedConstraints[1]->name(), "BC-name-2::hourly::hour<10>::low");
    BOOST_CHECK_EQUAL(violatedConstraints[2]->name(), "BC-name-1::hourly::hour<5>::low");
}

BOOST_AUTO_TEST_CASE(analysis_slack_variables_are_ordered_and_limited_to_10)
{
    std::unique_ptr<MPSolver> problem = createProblemWith_n_violatedConstraints(15);
    BOOST_CHECK(problem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(problem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());

    auto& violatedConstraints = analysis.largestSlackVariables();
    BOOST_CHECK_EQUAL(violatedConstraints.size(), 10);
    BOOST_CHECK_EQUAL(violatedConstraints[0]->name(), "BC-name-15::hourly::hour<75>::low");
    BOOST_CHECK_EQUAL(violatedConstraints[9]->name(), "BC-name-6::hourly::hour<30>::low");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(slack_variables_report)

BOOST_AUTO_TEST_CASE(constraints_associated_to_all_incoming_slack_vars_are_reported)
{
    // The problem is needed only to create variables, impossible otherwise.
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));

    std::vector<const operations_research::MPVariable*> slackVariables;
    slackVariables.push_back(problem->MakeNumVar(0, 1, "BC-1::hourly::hour<36>::low"));
    slackVariables.push_back(problem->MakeNumVar(0, 1, "BC-2::hourly::hour<65>::up"));
    slackVariables.push_back(
      problem->MakeNumVar(0, 1, "FictiveLoads::area<some-area>::hour<25>::low"));
    slackVariables.push_back(
      problem->MakeNumVar(0, 1, "HydroPower::area<some-area>::week<45>::up"));

    InfeasibleProblemReport report(slackVariables);
    report.storeSuspiciousConstraints();
    auto reportLogs = report.getLogs();

    BOOST_CHECK_EQUAL(reportLogs.size(), 5); // Expecting 5 lines in the report
    BOOST_CHECK_EQUAL(reportLogs[1], "Hourly BC 'BC-1' at hour 36");
    BOOST_CHECK_EQUAL(reportLogs[2], "Hourly BC 'BC-2' at hour 65");
    BOOST_CHECK_EQUAL(reportLogs[3], "Last resort shedding status at area 'some-area' at hour 25");
    BOOST_CHECK_EQUAL(reportLogs[4], "Hydro weekly production at area 'some-area'");
}

BOOST_AUTO_TEST_CASE(Infeasibility_causes_are_unique_and_sorted_by_slack_value)
{
    // The problem is needed only to create variables, impossible otherwise.
    std::unique_ptr<MPSolver> problem(MPSolver::CreateSolver("GLOP"));

    addOneVarConstraintToProblem(problem.get(), "HydroPower::area<cz00>::week<0>", 0., 1., 2.);
    addOneVarConstraintToProblem(problem.get(), "BC-1::hourly::hour<36>", 0., 1., 3.);
    addOneVarConstraintToProblem(problem.get(),
                                 "FictiveLoads::area<some-area>::hour<25>",
                                 0.,
                                 1.,
                                 4.);
    addOneVarConstraintToProblem(problem.get(), "BC-2::hourly::hour<65>", 0., 1., 5.);
    addOneVarConstraintToProblem(problem.get(),
                                 "FictiveLoads::area<some-area>::hour<56>",
                                 0.,
                                 1.,
                                 6.);

    BOOST_CHECK(problem->Solve() == MPSolver::INFEASIBLE);

    ConstraintSlackAnalysis analysis;
    analysis.run(problem.get());
    BOOST_CHECK(analysis.hasDetectedInfeasibilityCause());

    InfeasibleProblemReport report(analysis.largestSlackVariables());
    report.storeInfeasibilityCauses();
    auto reportLogs = report.getLogs();

    BOOST_CHECK_EQUAL(reportLogs.size(), 4); // Expecting 4 lines in the report
    BOOST_CHECK_EQUAL(reportLogs[0], "Possible causes of infeasibility:");
    BOOST_CHECK_EQUAL(reportLogs[1], "* Last resort shedding status.");
    BOOST_CHECK_EQUAL(reportLogs[2], "* Hourly binding constraints.");
    BOOST_CHECK_EQUAL(reportLogs[3], "* impossible to generate exactly the weekly hydro target");
}

BOOST_AUTO_TEST_SUITE_END()
