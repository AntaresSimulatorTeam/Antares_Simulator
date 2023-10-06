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
InfeasibleProblemAnalysis::InfeasibleProblemAnalysis(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx)
{
    problem_ = std::unique_ptr<MPSolver>(ProblemSimplexeNommeConverter(solverName, ProbSpx).Convert());
}

void InfeasibleProblemAnalysis::addSlackVariables()
{
    /* Optimization:
       We assess that less than 1 every 3 constraint will match
       the regex. If more, push_back may force the copy of memory blocks.
       This should not happen in most cases.
    */
    const unsigned int selectedConstraintsInverseRatio = 3;
    mSlackVariables.reserve(problem_->NumConstraints() / selectedConstraintsInverseRatio);
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
                mSlackVariables.push_back(slack);
            }

            if (constraint->ub() != infinity)
            {
                const MPVariable* slack
                  = problem_->MakeNumVar(0, infinity, constraint->name() + "::up");
                constraint->SetCoefficient(slack, -1.);
                mSlackVariables.push_back(slack);
            }
        }
    }
}

void InfeasibleProblemAnalysis::buildObjective() const
{
    MPObjective* objective = problem_->MutableObjective();
    // Reset objective function
    objective->Clear();
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack : mSlackVariables)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

MPSolver::ResultStatus InfeasibleProblemAnalysis::Solve() const
{
    return problem_->Solve();
}

bool InfeasibleProblemAnalysis::run()
{
    logs.notice() << " Solver: Starting infeasibility analysis...";
    addSlackVariables();
    if (mSlackVariables.empty())
    {
        logs.error() << "Cannot generate infeasibility report: no constraints have been selected";
        return false;
    }

    buildObjective();

    const MPSolver::ResultStatus status = Solve();
    if ((status != MPSolver::OPTIMAL) && (status != MPSolver::FEASIBLE))
    {
        logs.error() << "Linear problem could not be solved, and infeasibility analysis could not help";
        return false;
    }

    return true;
}

InfeasibleProblemReport InfeasibleProblemAnalysis::produceReport()
{
    return InfeasibleProblemReport(mSlackVariables);
}
} // namespace Optimization
} // namespace Antares
