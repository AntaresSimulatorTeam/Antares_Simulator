#include "problem.h"
#include <antares/logs/logs.h>

#include <fstream>
#include <regex>
#include <algorithm>

using namespace operations_research;

namespace Antares
{
namespace Optimization
{

SingleAnalysis::SingleAnalysis(std::shared_ptr<operations_research::MPSolver> problem)
    : problem_(problem)
{}

void SlackVariablesAnalysis::run()
{
    addSlackVariables();
    if (slackVariables_.empty())
    {
        logs.error() << "Slack variables analysis : no constraints have been selected";
        return;
    }

    buildObjective();

    const MPSolver::ResultStatus status = Solve();
    if ((status != MPSolver::OPTIMAL) && (status != MPSolver::FEASIBLE))
    {
        logs.error() << "Slack variables analysis : modified linear problem could not be solved";
        return;
    }

    hasDetectedInfeasibilityCause_ = true;
}

void SlackVariablesAnalysis::addSlackVariables()
{
    /* Optimization:
       We assess that less than 1 every 3 constraint will match
       the regex. If more, push_back may force the copy of memory blocks.
       This should not happen in most cases.
    */
    const unsigned int selectedConstraintsInverseRatio = 3;
    slackVariables_.reserve(problem_->NumConstraints() / selectedConstraintsInverseRatio);
    std::regex rgx(constraint_name_pattern);
    const double infinity = MPSolver::infinity();
    for (MPConstraint* constraint : problem_->constraints())
    {
        if (std::regex_search(constraint->name(), rgx))
        {
            if (constraint->lb() != -infinity)
            {
                const MPVariable* slack
                  = problem_->MakeNumVar(0, infinity, constraint->name() + "::low");
                constraint->SetCoefficient(slack, 1.);
                slackVariables_.push_back(slack);
            }

            if (constraint->ub() != infinity)
            {
                const MPVariable* slack
                  = problem_->MakeNumVar(0, infinity, constraint->name() + "::up");
                constraint->SetCoefficient(slack, -1.);
                slackVariables_.push_back(slack);
            }
        }
    }
}

void SlackVariablesAnalysis::buildObjective() const
{
    MPObjective* objective = problem_->MutableObjective();
    // Reset objective function
    objective->Clear();
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack : slackVariables_)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

MPSolver::ResultStatus SlackVariablesAnalysis::Solve() const
{
    return problem_->Solve();
}

void SlackVariablesAnalysis::printReport()
{
    InfeasibleProblemReport report(slackVariables_);
    report.prettyPrint();
}


InfeasibleProblemAnalysis::InfeasibleProblemAnalysis(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx)
{
    problem_ = std::unique_ptr<MPSolver>(ProblemSimplexeNommeConverter(solverName, ProbSpx).Convert());

    analysisList_.push_back(std::make_unique<SlackVariablesAnalysis>(problem_));
}

void InfeasibleProblemAnalysis::run()
{
    logs.notice() << " Solver: Starting infeasibility analysis...";

    for (auto& analysis : analysisList_)
    {
        analysis->run();
    }
}

void InfeasibleProblemAnalysis::printReport()
{
    for (auto& analysis : analysisList_)
    {
        if (analysis->hasDetectedInfeasibilityCause())
        {
            analysis->printReport();
            return;
        }
    }
}
} // namespace Optimization
} // namespace Antares
